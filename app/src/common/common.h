#pragma once
#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <cstring>

#define DMA_BUFFER_SIZE 1024
#define DMA_HALF_BUFFER_SIZE (DMA_BUFFER_SIZE / 2)
#define ADC_CHANNEL_COUNT (4)

#ifdef __cplusplus

#include <bit>
#include <concepts>
#include <type_traits>

template <std::integral T>
constexpr T ToLittleEndian(T value)
{
    if constexpr (std::endian::native == std::endian::little)
        return value;
    else
        return std::byteswap(value);
}

template <std::integral T>
constexpr T FromLittleEndian(T value) {
    return ToLittleEndian(value);
}

template <std::integral T>
constexpr T FromLittleEndian(const uint8_t* p) {
    T value;
    memcpy(&value, p, sizeof(T));
    
    if constexpr (std::endian::native == std::endian::big)
        value = std::byteswap(value);

    return value;
}

constexpr float ToLittleEndianFloat(float value)
{
    if constexpr (std::endian::native == std::endian::little)
        return value;
    else {
        union {
            float v;
            uint32_t g;
        } meow;

        meow.v = value;
        meow.g = std::byteswap(meow.g);
        return meow.v;
    }
}

constexpr float FromLittleEndianFloat(float value) {
    return ToLittleEndianFloat(value);
}



#endif