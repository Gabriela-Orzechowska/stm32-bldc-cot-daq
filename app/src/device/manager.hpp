#pragma once
#include "common/packet.h"
#include <vector>
#include <functional>
#include <span>

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
    Manager(){}
    ~Manager(){}

    static Manager& Get() {
        static Manager instance;
        return instance;
    }

    void Init();
    void Deinit();
    void SendConfig();

    inline config_t& GetConfig() { return m_deviceConfig; }

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

    config_t m_deviceConfig;
};
    
} // namespace Packet
