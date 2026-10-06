#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <thread>

#include <libusb-1.0/libusb.h>

namespace USB {
class Reader {
public:
    using DataCallback = std::function<void(const void* data, size_t size)>;
    
    Reader() = default;
    ~Reader() { this->Stop(); }

    Reader(const Reader&) = delete;
    Reader& operator=(const Reader&) = delete;

    inline bool IsRunning() const { return m_running; }
    inline int GetError() const { return m_error; }
    bool Start(libusb_device_handle *handle, uint8_t endpoint, DataCallback callback = nullptr);
    void Stop();

private:
    void ThreadTask();

    libusb_device_handle* m_device = nullptr;
    uint8_t m_endpoint = 0;

    DataCallback m_callback = nullptr;
    int m_error;

    std::atomic_bool m_running{false};
    std::thread m_thread;
};
}