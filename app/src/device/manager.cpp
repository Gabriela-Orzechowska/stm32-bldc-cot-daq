#include "device/manager.hpp"
#include "tcp/tcp.hpp"
#include "vendor/crc.h"
#include <pocketfft_hdronly.h>
#include <numeric>

namespace Device {
    void PacketParser::Feed(const std::span<const uint8_t> data) {
        this->m_buffer.insert(this->m_buffer.end(), data.begin(), data.end());

        while (true) {
            if (this->m_buffer.size() < 2)
                return;

            size_t magicPos = SIZE_MAX;
            for (size_t i = 0; i < this->m_buffer.size(); i++) {
                if (FromLittleEndian<uint16_t>(&m_buffer.data()[i]) == PACKET_HEADER_MAGIC) {
                    magicPos = i;
                    break;
                }
            }

            if (magicPos == SIZE_MAX) {
                uint8_t last = m_buffer.back();

                m_buffer.clear();
                if (last == uint8_t(PACKET_HEADER_MAGIC & 0xff))
                    m_buffer.push_back(last);

                return;
            }

            if (magicPos > 0) {
                this->m_buffer.erase(this->m_buffer.begin(), this->m_buffer.begin() + magicPos);
            }

            if (this->m_buffer.size() < sizeof(packet_header_t)) return;

            const uint8_t* p = this->m_buffer.data();

            uint16_t packetLenght = FromLittleEndian<uint16_t>(&p[2]);
            uint16_t packetType = FromLittleEndian<uint16_t>(&p[4]);

            if (packetType >= PACKET_TYPE_COUNT) return;
            if (this->m_buffer.size() < packetLenght) return;

			uint32_t crc = FromLittleEndian<uint32_t>(&p[packetLenght - 4]);
            uint32_t calculatedCrc = CRC32::MPEG_2::calc(p, packetLenght - 4);
			if (crc != calculatedCrc) {
				this->m_buffer.erase(this->m_buffer.begin(), this->m_buffer.begin() + packetLenght);
				return;
			}

            if (this->m_callback) {
                this->m_callback((const packet_header_t*)this->m_buffer.data());
            }

            this->m_buffer.erase(this->m_buffer.begin(), this->m_buffer.begin() + packetLenght);

        }
    }


    static std::vector<double> _CalculateFFT(std::vector<double>& input) {
        pocketfft::shape_t shape{input.size()};
        pocketfft::stride_t stride_in{(std::ptrdiff_t)sizeof(double)};
        pocketfft::stride_t stride_out{(std::ptrdiff_t)sizeof(std::complex<double>)};

        std::vector<double> output(input.size() / 2 + 1);
        std::vector<std::complex<double>> spectrum(input.size() / 2 + 1);
        pocketfft::r2c(shape, stride_in, stride_out, 0, pocketfft::FORWARD, input.data(), spectrum.data(), 1.0);

        for(size_t i = 0; i < output.size(); i++) {
            output[i] = std::abs(spectrum[i]);
        }

        return output;
    }

    void FFTHandler::ParseSpeed(const std::vector<double>& data) {
        if (this->m_speedSize == 0) throw std::invalid_argument("FFT size not set");

        std::lock_guard lock(m_mutex);
        this->m_speedSamples.insert(this->m_speedSamples.end(), data.begin(), data.end());
        while (this->m_speedSamples.size() > (this->m_speedSize + this->m_speedRevSize)) {
            this->m_speedSamples.erase(this->m_speedSamples.begin(), this->m_speedSamples.begin() + this->m_adcRevSize);
        }
    }

    void FFTHandler::ParseADC(const std::vector<uint16_t>& data) {
        if (this->m_adcSize == 0) throw std::invalid_argument("FFT size not set");
        assert(data.size() % ADC_CHANNEL_COUNT == 0);

        size_t nChan = data.size() / ADC_CHANNEL_COUNT;

        size_t start = this->m_adcSamples[0].size();
        for(size_t i = 0; i < ADC_CHANNEL_COUNT; i++) 
            this->m_adcSamples[i].resize(start + nChan);

        for(size_t n = 0; n < nChan; n++) {
            for(size_t j = 0; j < ADC_CHANNEL_COUNT; j++) {
                this->m_adcSamples[j][start + n] = data[ADC_CHANNEL_COUNT*n+j];
            }
        }

        while (this->m_adcSamples[0].size() > (this->m_adcSize + this->m_adcRevSize)) {
            for(size_t i = 0; i < ADC_CHANNEL_COUNT; i++ ) {
                this->m_adcSamples[i].erase(this->m_adcSamples[i].begin(), this->m_adcSamples[i].begin() + this->m_adcRevSize);
            }
        }
    }

    void FFTHandler::CalculateFFT() {
        if (this->m_speedSamples.size() >= this->m_speedSize) {
            std::vector<double> input(this->m_speedSamples.begin(), this->m_speedSamples.begin() + this->m_speedSize);

            double sum = std::reduce(input.begin(), input.end());
            double average = sum / (float)input.size();


            this->m_speedAverage = average;

            std::lock_guard lock(m_mutex);
            this->m_speedOutput = _CalculateFFT(input);
            this->m_speedOutput[0] = 0.0f;

            this->m_speedPlot = input;
            this->m_hasSpeedFFT = true;
        }

        for(int i = 0; i < 4; i++) {
            if (this->m_adcSamples[i].size() >= this->m_adcSize) {
                std::vector<double> input(this->m_adcSamples[i].begin(), this->m_adcSamples[i].begin() + this->m_adcSize);

                std::lock_guard lock(m_mutex);
                this->m_adcOutput[i] = _CalculateFFT(input);
                this->m_adcOutput[i][0] = 0.0f;

                this->m_adcPlot[i] = input;
            }
        }
    }

    static std::vector<double> _CalculateSpeed(const std::vector<uint32_t> samples, uint32_t clockSpeed, uint32_t sampleCount) {
        std::vector<double> output(samples.size(), 0);

        // Something broke so gonna fix later
        const double mult = (60.0 * (double)clockSpeed) / (4.0 * (double)sampleCount);
        for(size_t i = 0; i < samples.size(); i++) {
            output[i] = mult / (double)samples[i];
        }

        return output;
    }

    void Manager::UpdatePaths() {
        this->m_timerValuePath =  this->m_outputPath / "speed.bin";
        this->m_adcValuesPath = this->m_outputPath / "adc.bin";
    }

    void Manager::BeginCapture() {
        if (this->m_isCapturing) return;

        this->m_fft.SetupFFT(
            this->m_deviceConfig.encoder_resolution, 
            this->m_deviceConfig.encoder_resolution / this->m_deviceConfig.adc_div, 
            this->m_rotationsToAnalise
        );

        this->m_isCapturing = true;

        this->m_timerValueFile.open(this->m_timerValuePath, std::ios::out | std::ios::trunc | std::ios::binary);
        this->m_adcValuesFile.open(this->m_adcValuesPath, std::ios::out | std::ios::trunc | std::ios::binary);

        this->m_fftThread = std::thread([this] {
            for(;;) {
                if (!this->m_isCapturing) return;
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                this->m_fft.CalculateFFT();
            }
        });
    }

    void Manager::EndCapture() {
        if (!this->m_isCapturing) return;

        this->m_isCapturing = false;

        this->m_timerValueFile.flush();
        this->m_adcValuesFile.flush();

        this->m_timerValueFile.close();
        this->m_adcValuesFile.close();

        if (this->m_fftThread.joinable())
            this->m_fftThread.join();
    }

    static std::optional<std::filesystem::path> _GetUserFolder() {
#ifdef _WIN32
        const char* home = std::getenv("USERPROFILE");
#else
        const char* home = std::getenv("HOME");
#endif

        if (home)
            return std::filesystem::path(home);

        return std::nullopt;
    }

    void Manager::Init() {
        this->m_parser.SetCallback(&Manager::__ProcessPacket);

        TCP::ClientManager::Get().SetDataCallback(&Manager::__OnDataReceive);
        
        auto user = _GetUserFolder();
        if (user.has_value()) {
            this->m_outputPath = user.value() / "cot-daq-data";
            std::filesystem::create_directory(this->m_outputPath);
            this->UpdatePaths();
        }

    }

    void Manager::_OnDataReceive(const std::span<uint8_t>& data) {
        this->m_parser.Feed(data);
    }

    static uint32_t _GetADCResolution(const packet_adc_res_t res) {
        switch(res) {
            case ADC_RES_8:
                return 0xFF;
            case ADC_RES_10:
                return 0x3FF;
            case ADC_RES_12:
                return 0xFFF;
            case ADC_RES_14:
                return 0x3FFF;
            case ADC_RES_16:
                return 0xFFFF;
        }
        return 0;
    }

    static std::vector<uint32_t> _ProcessDeltas(const packet_header_t* header) {
        std::vector<uint32_t> deltas(DMA_HALF_BUFFER_SIZE, 0);

        const uint16_t* raw = (const uint16_t*)header + (sizeof(packet_header_t)/sizeof(uint16_t));
        const uint16_t* end = raw + (header->length - sizeof(packet_header_t) - sizeof(uint32_t)) / (sizeof(uint16_t));

        size_t i = 0;
        while (raw < end) {
            uint32_t delta;
            const uint16_t d = *raw++;
            if (d == 0xFFFFu) {
                const uint16_t high = *raw++;
                const uint16_t low  = *raw++;
                delta = (FromLittleEndian<uint16_t>(high) << 16) | FromLittleEndian<uint16_t>(low);
            }
            else {
                delta = FromLittleEndian<uint16_t>(d);
            }

            deltas[i++] = delta;
            if (i >= DMA_HALF_BUFFER_SIZE) break;
        }
        return deltas;
	}

    static std::vector<uint16_t> _ExtractADCValues(const packet_header_t* header) {
        const size_t count = (header->length - sizeof(packet_simple_t)) / sizeof(uint16_t);
        std::vector<uint16_t> values(count, 0);

        const uint16_t* start = (const uint16_t*)header + (sizeof(packet_header_t) / sizeof(uint16_t));
        for (size_t i = 0; i < count; i++) {
            values[i] = FromLittleEndian(start[i]);
        }

        return values;
    }

    void Manager::_ProcessPacket(const packet_header_t* header) {
        switch (header->type) {
        case PACKET_TYPE_CONFIG: {
            const packet_config_t* config = (const packet_config_t*) header;
            
			auto& deviceConfig = this->m_deviceConfig;
			// clang-format off
            deviceConfig.magic              = FromLittleEndian<uint32_t>(config->config.magic);
            deviceConfig.encoder_mode       = config->config.encoder_mode;
            deviceConfig.encoder_channel    = config->config.encoder_channel;
            deviceConfig.encoder_resolution = FromLittleEndian<uint16_t>(config->config.encoder_resolution);
            deviceConfig.adc_div            = config->config.adc_div;
            deviceConfig.interlace          = config->config.interlace;
            deviceConfig.crc                = FromLittleEndian<uint32_t>(config->config.crc);

            for (int i = 0; i < ADC_CHANNEL_COUNT; i++) {
                deviceConfig.adc_config[i].enabled = config->config.adc_config[i].enabled;
                deviceConfig.adc_config[i].max_val = FromLittleEndianFloat(config->config.adc_config[i].max_val);
                deviceConfig.adc_config[i].min_val = FromLittleEndianFloat(config->config.adc_config[i].min_val);
                memcpy(deviceConfig.adc_config[i].unit, config->config.adc_config[i].unit, 3);
            }
			// clang-format on
        } break;
        case PACKET_TYPE_DEVICE_INFO: {
			const packet_device_info_t* info = (const packet_device_info_t*)header;
            auto& deviceInfo = this->m_deviceInfo;

            char name[5] = { 0 };
			uint32_t rawName = std::byteswap(FromLittleEndian<uint32_t>(info->device.name));
            memcpy(name, &rawName, sizeof(uint32_t));

			this->m_deviceName =  name;
			
            deviceInfo.clock = FromLittleEndian<uint32_t>(info->device.clock);
            deviceInfo.adc_num = info->device.adc_num;
            deviceInfo.adc_res = info->device.adc_res; 

            this->m_adcResolution = _GetADCResolution((packet_adc_res_t) info->device.adc_res);

		} break;
        case PACKET_TYPE_TIMER_DELTA: {
            if (!this->m_isCapturing) break;

            auto ret = _ProcessDeltas(header);

            auto speed = _CalculateSpeed(ret, m_deviceInfo.clock, m_deviceConfig.encoder_resolution);
            this->m_fft.ParseSpeed(speed);
            this->m_timerValueFile.write((const char*)(ret.data()), ret.size() * sizeof(uint32_t));
        } break;
        case PACKET_TYPE_ADC_VALUES: {
            if (!this->m_isCapturing) break;

            auto ret = _ExtractADCValues(header);
            this->m_fft.ParseADC(ret);
            this->m_adcValuesFile.write((const char*)ret.data(), ret.size() * sizeof(uint16_t));
        } break;

        default:
            return;
        }
    }

    void Manager::SendConfig() {
        packet_config_t packet;

        packet.header.magic = ToLittleEndian<uint16_t>(PACKET_HEADER_MAGIC);
        packet.header.length = ToLittleEndian<uint16_t>(sizeof(packet_config_t));
        packet.header.type = ToLittleEndian<uint16_t>(PACKET_TYPE_CONFIG);
        packet.header.reserved = 0;

        auto& deviceConfig = this->m_deviceConfig;
        // clang-format off
		packet.config.magic = ToLittleEndian<uint32_t>(CONFIG_MAGIC);
        packet.config.encoder_mode = deviceConfig.encoder_mode;
        packet.config.encoder_channel = deviceConfig.encoder_channel;
        packet.config.encoder_resolution = ToLittleEndian<uint16_t>(deviceConfig.encoder_resolution);
        packet.config.adc_div = deviceConfig.adc_div;
        packet.config.interlace = deviceConfig.interlace;

        for (int i = 0; i < ADC_CHANNEL_COUNT; i++) {
            packet.config.adc_config[i].enabled = deviceConfig.adc_config[i].enabled;
            packet.config.adc_config[i].max_val = ToLittleEndianFloat(deviceConfig.adc_config[i].max_val);
            packet.config.adc_config[i].min_val = ToLittleEndianFloat(deviceConfig.adc_config[i].min_val);
            memcpy(packet.config.adc_config[i].unit, deviceConfig.adc_config[i].unit, 3);
        }

		packet.config.crc = ToLittleEndian<uint32_t>(CRC32::MPEG_2::calc((const uint8_t*)&packet.config, PACKET_RAW_SIZE(config_t)));

        packet.crc = FromLittleEndian<uint32_t>(CRC32::MPEG_2::calc((const uint8_t*)&packet, PACKET_RAW_SIZE(packet_config_t)));
        // clang-format on

        std::span<uint8_t> data((uint8_t*)&packet, sizeof(packet_config_t));
        TCP::ClientManager::Get().SendData(data);
    }
}