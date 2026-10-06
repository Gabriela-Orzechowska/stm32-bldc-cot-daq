#include "usb_device.hpp"
#include <iostream>
#include <chrono>

namespace USB {
bool Device::Start(DataCallback cb) {
    if (this->m_isRunning) return false;

    if (libusb_init(&this->m_context) != LIBUSB_SUCCESS) return false;
    
    this->m_dataCallback = std::move(cb);
    this->m_isRunning = true;

    this->m_thread = std::thread(&Device::ThreadTask, this);

    return true;
}

void Device::Stop() {
    this->m_isRunning = false;

    if (this->m_thread.joinable()) this->m_thread.join();
    this->CloseDevice();

    if (this->m_context) {
        libusb_exit(this->m_context);
        this->m_context = nullptr;
    }
}

bool Device::FindDevice() {
    libusb_device** devices = nullptr;

    const ssize_t deviceCount = libusb_get_device_list(this->m_context, &devices);
    if (deviceCount < 0) return false;

    bool found = false;
    for(ssize_t i = 0; i < deviceCount; i++) {
        libusb_device* device = devices[i];
        libusb_device_descriptor descriptor{};
        libusb_device_handle* handle = nullptr;

        if (libusb_get_device_descriptor(device, &descriptor) != LIBUSB_SUCCESS)
            continue;

        if (descriptor.idProduct != this->m_pid || descriptor.idVendor != this->m_vid)
            continue;

        if (libusb_open(device, &handle) != LIBUSB_SUCCESS)
            continue;

        if (libusb_claim_interface(handle, 0) != LIBUSB_SUCCESS) {
            libusb_close(handle);
            continue;
        }

        this->m_device = handle;
        found = true;
        break;
    }

    libusb_free_device_list(devices, 1);
    return found;
}

void Device::CloseDevice() {
    if (!this->m_device) return;

    this->m_reader.Stop();
    libusb_release_interface(this->m_device, 0);
    libusb_close(this->m_device);
    this->m_device = nullptr;

    if (this->m_isConnected) {
        this->m_isConnected = false;
        if (this->m_disconnectCallback) this->m_disconnectCallback();
    }
}

void Device::ThreadTask() {
    while (this->m_isRunning) {

        if (!this->m_isConnected) {
            if (this->FindDevice()) {
                this->m_isConnected = true;
                if (this->m_connectCallback) this->m_connectCallback();

                this->m_reader.Start(this->m_device, this->m_endpoint, this->m_dataCallback);
            }
        }
        else {
            if (!this->m_reader.IsRunning()) {
                this->CloseDevice();
            }
        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(250)
        );
    }

    this->CloseDevice();
}
}