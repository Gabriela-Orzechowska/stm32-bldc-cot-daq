#include "tcp.hpp"

namespace TCP {
ClientManager::ClientManager() :
    m_strand(asio::make_strand(m_context)),
    m_resolver(m_context),
    m_socket(m_context) 
{
    m_thread = std::thread([this] { this->m_context.run(); });
}

ClientManager::~ClientManager() {
    this->m_context.stop();

    if (this->m_thread.joinable())
        this->m_thread.join();
}

void ClientManager::TryConnect() {
    this->m_connecting = true;
    asio::post(this->m_strand,
        [this] {
            if (this->m_stopping || this->m_connected) return;

            this->m_resolver.async_resolve("192.168.1.130", "5000",
                asio::bind_executor(this->m_strand,
                [this](const asio::error_code& ec, tcp::resolver::results_type endpoints) {
                    this->m_connecting = false;
                    if (ec) {
                        // TODO Add error handling
                        return;
                    }
                    asio::async_connect(this->m_socket, endpoints,
                    asio::bind_executor(this->m_strand,
                    [this](const asio::error_code& ec, const tcp::endpoint&) {
                        if (ec) {
                            // TODO Add error handling;
                            return;
                        }
                        this->m_connected = true;
                        this->ProcessRead();
                        
                    }));
                }));
        }
    );
}

void ClientManager::TryDisconnect() {
    asio::post(
        this->m_strand,
        [this] {
            this->m_connected = false;
            this->m_resolver.cancel();
            
            asio::error_code error;
            this->m_socket.close(error);

            this->m_writeQueue.clear();
        });
}

void ClientManager::SendData(const std::span<uint8_t> &data) {
    if(data.size() == 0) return;

    auto dataP = std::make_shared<std::span<uint8_t>>(data);

    asio::post(
        this->m_strand,
        [this, dataP] {
            if (this->m_stopping || !this->m_connected) return;

            bool isIdle = this->m_writeQueue.empty();

            this->m_writeQueue.push_back(dataP);

            if (isIdle) this->ProcessWrite();
        });
}

void ClientManager::ProcessRead() {
    this->m_socket.async_read_some(
        asio::buffer(this->m_readBuffer),
        asio::bind_executor(
            this->m_strand,
            [this](const asio::error_code& ec, size_t size) {
                if (ec) {
                    this->m_connected = false;
                    return;
                }

                std::span<uint8_t> data(this->m_readBuffer.begin(),
                    this->m_readBuffer.begin() + size);

                if (this->m_dataCallback)
                    this->m_dataCallback(data);
                
                this->ProcessRead();
        }));
}

void ClientManager::ProcessWrite() {
    if (!this->m_connected || this->m_writeQueue.empty()) return;

    auto data = this->m_writeQueue.front();
    
    asio::async_write(
        this->m_socket,
        asio::buffer(*data),
        asio::bind_executor(
            this->m_strand,
            [this, data](const asio::error_code& ec, size_t) {
                if (ec) {
                    this->m_connected = false;
                    return;
                }

                this->m_writeQueue.pop_front();
                this->ProcessWrite();
            }));
}
}