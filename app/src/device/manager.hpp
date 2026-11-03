#pragma once
#include "common/packet.h"
#include <string>
#include <vector>
#include <functional>
#include <span>
#include <filesystem>
#include <fstream>
#include <thread>

#define MAX_PATH 260

namespace Render {
    class ImGuiManager;
}

namespace Device {
class PacketParser {
public:
    using PacketCallback = std::function<void(const packet_header_t*)>;

    PacketParser(){}
    ~PacketParser(){}
    
    void SetCallback(PacketCallback cb) {
        this->m_callback = cb;
    }
    void Feed(const std::span<const uint8_t> data);

private:
    std::vector<uint8_t> m_buffer;
    PacketCallback m_callback = nullptr;
};

class FFTHandler {
public:
    FFTHandler() : m_hasSpeedFFT(false) {};
    ~FFTHandler(){};

    inline void SetupFFT(size_t speedSize, size_t adcSize) {
        this->m_speedSize = speedSize;
        this->m_adcSize = adcSize;
    }

    void ParseSpeed(const std::vector<double>& data);
    void ParseADC(const std::vector<uint16_t>& data);
    void CalculateFFT();

    inline bool HasSpeedFFT() const { return m_hasSpeedFFT; }

    inline const std::vector<double> GetSpeedFFT() const { 
        return m_speedOutput; 
    }
    inline const std::vector<double> GetADCFFT(int i) const {
        return m_adcOutput[i];
    }

protected:
    size_t m_speedSize;
    size_t m_adcSize;

    std::vector<double> m_speedSamples;
    std::vector<double> m_speedOutput;

    std::vector<double> m_adcSamples[4];
    std::vector<double> m_adcOutput[4];

    bool m_hasSpeedFFT;

};

class Manager {
public:
    friend class Render::ImGuiManager;

    Manager(){}
    ~Manager(){
        this->EndCapture();
    }

    static Manager& Get() {
        static Manager instance;
        return instance;
    }

    void Init();
    void Deinit();
    void SendConfig();
    void UpdatePaths();

    void BeginCapture();
    void EndCapture();

    inline const FFTHandler& GetFFT() const { return m_fft; }
    inline config_t& GetConfig() { return m_deviceConfig; }

	inline const std::string& GetDeviceName() const { return m_deviceName; }
	inline uint32_t GetDeviceClock() const { return m_deviceClock; }

private:
    void _ProcessPacket(const packet_header_t *header);
    void _OnDataReceive(const std::span<uint8_t> &data);

    static void _OnConnect();
    static void _OnDisconnect();
    static void __OnDataReceive(const std::span<uint8_t> &data) {
        Manager::Get()._OnDataReceive(data);
    }
    static void __ProcessPacket(const packet_header_t* header) {
        Manager::Get()._ProcessPacket(header);
    }

    PacketParser m_parser;
    FFTHandler m_fft;
    bool m_isCapturing;

    std::filesystem::path m_outputPath;
    config_t m_deviceConfig;
    std::string m_deviceName = "";
	uint32_t m_deviceClock = 0;

    std::filesystem::path m_timerValuePath;
    std::ofstream m_timerValueFile;

    std::filesystem::path m_adcValuesPath;
    std::ofstream m_adcValuesFile;

    std::thread m_fftThread;
};
} // namespace Device
