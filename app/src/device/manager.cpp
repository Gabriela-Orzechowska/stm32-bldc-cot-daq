#include "device/manager.hpp"
#include "tcp/tcp.hpp"
#include "vendor/crc.h"

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

    void Manager::UpdatePaths() {
        this->m_timerValuePath =  this->m_outputPath / "speed.bin";
        this->m_adcValuesPath = this->m_outputPath / "adc.bin";
    }

    void Manager::BeginCapture() {
        if (this->m_isCapturing) return;

        this->m_isCapturing = true;

        this->m_timerValueFile.open(this->m_timerValuePath, std::ios::out | std::ios::trunc | std::ios::binary);
        this->m_adcValuesFile.open(this->m_adcValuesPath, std::ios::out | std::ios::trunc | std::ios::binary);
    }

    void Manager::EndCapture() {
        if (!this->m_isCapturing) return;

        this->m_isCapturing = false;

        this->m_timerValueFile.flush();
        this->m_adcValuesFile.flush();

        this->m_timerValueFile.close();
        this->m_adcValuesFile.close();
    }

    void Manager::Init() {
        this->m_parser.SetCallback(&Manager::__ProcessPacket);

        TCP::ClientManager::Get().SetDataCallback(&Manager::__OnDataReceive);
    }

    void Manager::_OnDataReceive(const std::span<uint8_t>& data) {
        this->m_parser.Feed(data);
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
            deviceConfig.crc                = FromLittleEndian<uint32_t>(config->config.crc);
			// clang-format on
        } break;
        case PACKET_TYPE_DEVICE_INFO: {
			const packet_device_info_t* info = (const packet_device_info_t*)header;

            char name[5] = { 0 };
			uint32_t rawName = std::byteswap(FromLittleEndian<uint32_t>(info->device_name));

            memcpy(name, &rawName, sizeof(uint32_t));
			this->m_deviceName =  name;
			this->m_deviceClock = FromLittleEndian<uint32_t>(info->device_clock);
		} break;
        case PACKET_TYPE_TIMER_DELTA: {
            if (!this->m_isCapturing) break;

            auto ret = _ProcessDeltas(header);
            this->m_timerValueFile.write((const char*)(ret.data()), ret.size() * sizeof(uint32_t));
        } break;
        case PACKET_TYPE_ADC_VALUES: {
            if (!this->m_isCapturing) break;

            auto ret = _ExtractADCValues(header);
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
		packet.config.crc = ToLittleEndian<uint32_t>(CRC32::MPEG_2::calc((const uint8_t*)&packet.config, PACKET_RAW_SIZE(config_t)));

        packet.crc = FromLittleEndian<uint32_t>(CRC32::MPEG_2::calc((const uint8_t*)&packet, PACKET_RAW_SIZE(packet_config_t)));
        // clang-format on

        std::span<uint8_t> data((uint8_t*)&packet, sizeof(packet_config_t));
        TCP::ClientManager::Get().SendData(data);
    }
}