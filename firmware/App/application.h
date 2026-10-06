#pragma once
#include "types.h"

#define DMA_BUFFER_SIZE 1024
#define DMA_HALF_BUFFER_SIZE (DMA_BUFFER_SIZE / 2)

#define PACKET_TYPE_ENCODER_DELTA		(0)
#define PACKET_TYPE_ADC_VALUES			(1)

typedef struct {
	uint16_t magic;
	uint16_t length;
	uint8_t type;
	uint8_t subtype;
	uint16_t reserved;
} payload_header_t;

typedef struct {
	payload_header_t header;
	uint32_t deltas[DMA_HALF_BUFFER_SIZE];
	uint32_t crc;
} payload_timer_delta_t;

typedef enum {
	ENCODER_A_RISING,
	ENCODER_A_FALLING,
	ENCODER_B_RISING,
	ENCODER_B_FALLING,

	ENCODER_CHANNEL_COUNT,
} encoder_channel_t;


void application_entry(void);
