#include "manager.hpp"
#include "tcp/tcp.hpp"

namespace Device {
void PacketParser::Feed(const std::span<const uint8_t> data) {
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

    TCP::ClientManager::Get().SetDataCallback(&Manager::__OnDataReceive);
}

void Manager::_OnDataReceive(const std::span<uint8_t>& data) {
    this->m_parser.Feed(data);
}

}