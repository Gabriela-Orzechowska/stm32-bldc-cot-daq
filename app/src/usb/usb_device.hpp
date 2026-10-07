#pragma once

#include <atomic>
#include <thread>
#include <functional>
#include <cstdint>
#include <libusb-1.0/libusb.h>

#include "usb_reader.hpp"

namespace USB {
class Device {
public:
    using DeviceCallback = std::function<void()>;
    using DataCallback = std::function<void(const void* data, size_t size)>;

    Device(uint16_t vid, uint16_t pid, uint8_t endpoint, uint8_t sendEndpoint) :
        m_vid(vid), m_pid(pid), m_endpoint(endpoint), m_sendEndpoint(sendEndpoint) {}
    ~Device() { this->Stop(); }

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;
    
    bool Start(DataCallback callback);
    void Stop();

    bool SendData(const void* data, size_t size);

    inline bool IsConnected() const { return m_isConnected; }
    inline void SetConnectCallback(DeviceCallback cb) { m_connectCallback = cb; }
    inline void SetDisconnectCallback(DeviceCallback cb) { m_disconnectCallback = cb; }

private:
    bool FindDevice();
    void CloseDevice();
    void ThreadTask();

    uint16_t m_vid;
    uint16_t m_pid;
    uint8_t m_endpoint;
    uint8_t m_sendEndpoint;

    libusb_context* m_context = nullptr;
    libusb_device_handle* m_device = nullptr;

    Reader m_reader;
    DataCallback m_dataCallback;

    DeviceCallback m_connectCallback;
    DeviceCallback m_disconnectCallback;

    std::atomic_bool m_isConnected{false};
    std::atomic_bool m_isRunning{false};

    std::thread m_thread;
};
}