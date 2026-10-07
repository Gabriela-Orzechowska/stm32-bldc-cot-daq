#pragma once
#include "common.h"
#include "config.h"

#define PACKET_HEADER_MAGIC 0x55AA
#define PACKET_RAW_SIZE(x) (sizeof(x) - sizeof(uint32_t))

typedef enum {
	PACKET_TYPE_TIMER_DELTA,
	PACKET_TYPE_ADC_VALUES,
	PACKET_TYPE_CONFIG,
	PACKET_TYPE_CONFIG_REQUEST,

	PACKET_TYPE_COUNT,
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

typedef struct {
	packet_header_t header;
	config_t config;
	uint32_t crc;
} packet_config_t;

typedef struct {
	packet_header_t header;
	uint32_t crc;
} packet_simple_t;

typedef void(*packet_callback)(packet_type_t, const void*, size_t);
void packet_init(packet_callback cb);

void packet_timer_delta_init(packet_timer_delta_t* packet);
void packet_timer_delta_fill(packet_timer_delta_t* packet, const uint32_t* data, uint32_t *last_capture);

void packet_config_init(packet_config_t *packet);
