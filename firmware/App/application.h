#pragma once
#include "types.h"

#define DMA_BUFFER_SIZE 1024
#define DMA_HALF_BUFFER_SIZE (DMA_BUFFER_SIZE / 2)

typedef struct {
	uint16_t header;
	uint16_t length;
} payload_header_t;

typedef struct {
	payload_header_t header;
	uint32_t deltas[DMA_HALF_BUFFER_SIZE];
} payload_timer_delta_t;

typedef enum {
	ENCODER_A_RISING,
	ENCODER_A_FALLING,
	ENCODER_B_RISING,
	ENCODER_B_FALLING,

	ENCODER_CHANNEL_COUNT,
} encoder_channel_t;


void application_entry(void);
