#pragma once
#include "common.h"

typedef enum {
	PACKET_TYPE_TIMER_DELTA,
	PACKET_TYPE_ADC_VALUES,
	PACKET_TYPE_CONFIG,
} packet_type_t;

typedef struct {
	uint16_t magic;
	uint16_t length;
	uint16_t type;
	uint16_t reserved;
} packet_header_t;

typedef struct {
	packet_header_t header;
	uint32_t deltas[DMA_HALF_BUFFER_SIZE];
	uint32_t crc;
} packet_timer_delta_t;

void packet_timer_delta_init(packet_timer_delta_t* packet);
void packet_timer_delta_fill(packet_timer_delta_t* packet, const uint32_t* data, uint32_t *last_capture);
