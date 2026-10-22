#pragma once
#include "types.h"

#define ALIGN_NO_CACHE __attribute__((section(".no-cache"), aligned(0x20)))

#define DMA_BUFFER_SIZE 1024
#define DMA_HALF_BUFFER_SIZE (DMA_BUFFER_SIZE / 2)

