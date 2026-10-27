#pragma once
#include <functional>
#include <thread>
#include <vector>
#include <memory>
#include <string>
#include <deque>
#include <asio.hpp>

namespace TCP {
class ClientManager {
public:
    using DataCallback = std::function<void(std::span<uint8_t>&)>;

    static ClientManager& Get() {
        static ClientManager instance;
        return instance;
    }

    ClientManager();
    ~ClientManager();

    void Init();
    void TryConnect();
    void TryDisconnect();

    void SendData(const std::span<uint8_t>& data);

    inline void SetDataCallback(DataCallback cb) {
        this->m_dataCallback = cb;
    }
    inline bool IsConnected() const {
        return this->m_connected;
    }
    inline bool IsConnecting() const {
        return this->m_connecting;
    }

protected:
    void ProcessRead();
    void ProcessWrite();

    using tcp = asio::ip::tcp;

    asio::io_context m_context;
    asio::strand<asio::io_context::executor_type> m_strand;

    tcp::resolver m_resolver;
    tcp::socket m_socket;
    
    std::thread m_thread;
    std::array<uint8_t, 64 * 1024> m_readBuffer;
    std::deque<std::shared_ptr<std::span<uint8_t>>> m_writeQueue;

    DataCallback m_dataCallback = nullptr;

    bool m_connecting = false;
    bool m_connected = false;
    bool m_stopping = false;
};
}