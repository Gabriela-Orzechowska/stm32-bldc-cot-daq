#pragma once
#include "common/packet.h"
#include <string>
#include <vector>
#include <functional>
#include <span>
#include <filesystem>
#include <fstream>

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
    bool m_isCapturing;

    std::filesystem::path m_outputPath;
    config_t m_deviceConfig;
    std::string m_deviceName = "";
	uint32_t m_deviceClock = 0;

    std::filesystem::path m_timerValuePath;
    std::ofstream m_timerValueFile;

    std::filesystem::path m_adcValuesPath;
    std::ofstream m_adcValuesFile;
};
} // namespace Device
