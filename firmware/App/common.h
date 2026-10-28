#pragma once
#include "types.h"

#define ALIGN_NO_CACHE_D2 __attribute__((section(".no_cache"), aligned(0x20)))
#define ALIGN_NO_CACHE_D3 __attribute__((section(".no_cache_D3"), aligned(0x20)))
#define ADC_CHANNEL_COUNT (4)

#define DMA_BUFFER_SIZE (1024)
#define DMA_HALF_BUFFER_SIZE (DMA_BUFFER_SIZE / 2)

