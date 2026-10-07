#pragma once

#include "usb_device.hpp"
#include "common/packet.h"

namespace USB {
class PacketParser {
public:
    using PacketCallback = std::function<void(const packet_header_t*)>;

    PacketParser(){}
    ~PacketParser(){}
    
    void SetCallback(PacketCallback cb) {
        this->m_callback = cb;
    }
    void Feed(std::span<const uint8_t> data);

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
    inline bool IsConnected() const { return m_device.IsConnected(); }

private:
    enum class PacketState {
        WAIT_MAGIC,
        READ_HEADER,
        READ_PAYLOAD,
    };

    void _ProcessPacket(const packet_header_t *header);
    void _OnDataReceive(const void* data, size_t size);

    static void _OnConnect();
    static void _OnDisconnect();
    static void __OnDataReceive(const void* data, size_t size) {
        Manager::Get()._OnDataReceive(data, size);
    }
    static void __ProcessPacket(const packet_header_t* header) {
        Manager::Get()._ProcessPacket(header);
    }


    Device m_device{0xCAFE, 0x4006, 0x81, 0x01};
    PacketParser m_parser;

    config_t m_deviceConfig;
};
}