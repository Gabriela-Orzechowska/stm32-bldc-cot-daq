#include "usb_manager.hpp"
#include <common/packet.h>
#include <vendor/crc.h>

namespace USB {
void PacketParser::Feed(std::span<const uint8_t> data) {
    this->m_buffer.insert(this->m_buffer.end(), data.begin(), data.end());

    while (true) {
        if (this->m_buffer.size() < 2)
            return;

        size_t magicPos = SIZE_MAX;
        for(size_t i = 0; i < this->m_buffer.size(); i++) {
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

        uint16_t packetLenght   = FromLittleEndian<uint16_t>(&p[2]);
        uint16_t packetType     = FromLittleEndian<uint16_t>(&p[4]);

        if (packetType >= PACKET_TYPE_COUNT) return;
        if (this->m_buffer.size() < packetLenght) return;
        
        if (this->m_callback) {
           this->m_callback((const packet_header_t*) this->m_buffer.data());
        }

        this->m_buffer.erase(this->m_buffer.begin(), this->m_buffer.begin() + packetLenght);

    }
}

void Manager::Init() {
    this->m_parser.SetCallback(&Manager::__ProcessPacket);

    this->m_device.SetConnectCallback(&Manager::_OnConnect);
    this->m_device.SetDisconnectCallback(&Manager::_OnDisconnect);
    this->m_device.Start(&Manager::__OnDataReceive);
}

void Manager::Deinit() {
    this->m_device.Stop();
}

void Manager::SendConfig() {
    packet_config_t packet;
    packet.header.magic        = ToLittleEndian<uint16_t>(PACKET_HEADER_MAGIC);
    packet.header.length       = ToLittleEndian<uint16_t>(sizeof(packet_config_t)); 
    packet.header.type         = ToLittleEndian<uint16_t>(PACKET_TYPE_CONFIG);
    packet.header.reserved     = 0;

    packet.config = this->m_deviceConfig;

    packet.crc = CRC32::MPEG_2::calc((const uint8_t*) &packet, PACKET_RAW_SIZE(packet));

    Manager::Get().m_device.SendData(&packet, sizeof(packet));
}

void Manager::_OnConnect() {
    packet_simple_t packet;

    packet.header.magic        = ToLittleEndian<uint16_t>(PACKET_HEADER_MAGIC);
    packet.header.length       = ToLittleEndian<uint16_t>(sizeof(packet_simple_t)); 
    packet.header.type         = ToLittleEndian<uint16_t>(PACKET_TYPE_CONFIG_REQUEST);
    packet.header.reserved     = 0;

    packet.crc = CRC32::MPEG_2::calc((const uint8_t*) &packet, PACKET_RAW_SIZE(packet));

    Manager::Get().m_device.SendData(&packet, sizeof(packet));
}

void Manager::_OnDisconnect() {}

void Manager::_ProcessPacket(const packet_header_t* header) {
    switch(header->type) {
        case PACKET_TYPE_CONFIG: {
            const packet_config_t *packet = (const packet_config_t*) header;
            uint32_t calculatedCRC = CRC32::MPEG_2::calc((const uint8_t*)packet, PACKET_RAW_SIZE(packet_config_t));
            if (packet->crc != calculatedCRC) {
                std::printf("Invalid CRC. Expected %08X, got %08X\n", packet->crc, calculatedCRC);
                return;
            }
            
            this->m_deviceConfig = packet->config;
            std::printf("Received config\n");
        } break;
    }
}

void Manager::_OnDataReceive(const void* buff, size_t size) {
    this->m_parser.Feed({(const uint8_t*)buff, size});
}

}