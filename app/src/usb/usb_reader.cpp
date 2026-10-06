#include "usb_reader.hpp"

namespace USB {

bool Reader::Start(libusb_device_handle *handle, uint8_t endpoint, DataCallback callback) {
    if (!handle || this->m_running) return false;

    this->m_device = handle;
    this->m_endpoint = endpoint;
    this->m_callback = callback;

    this->m_running = true;
    this->m_thread = std::thread(&Reader::ThreadTask, this);

    return true;
}

void Reader::Stop() {
    this->m_running = false;

    if (this->m_thread.joinable())
        this->m_thread.join();

    this->m_device = nullptr;
    this->m_callback = nullptr;
}

void Reader::ThreadTask() {
    std::array<uint8_t, 4096> buf{};

    while(this->m_running) {
        int transferred;

        this->m_error = libusb_bulk_transfer(
            this->m_device,
            this->m_endpoint,
            buf.data(),
            (int) buf.size(),
            &transferred,
            100
        );

        if (!this->m_running) break;

        switch(this->m_error) {
            case LIBUSB_SUCCESS: {
                if (transferred > 0 && this->m_callback) {
                    this->m_callback(buf.data(), (size_t) transferred);
                } break;
            };
            case LIBUSB_ERROR_TIMEOUT: break;

            case LIBUSB_ERROR_NO_DEVICE: 
            default: 
                this->m_running = false;
                break;
        }
    }

    this->m_running = false;
}
}