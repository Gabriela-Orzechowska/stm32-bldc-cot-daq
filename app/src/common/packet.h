#pragma once
#include "common.h"
#include "config.h"

#define PACKET_HEADER_MAGIC 0x55AA
#define PACKET_RAW_SIZE(x) (sizeof(x) - sizeof(uint32_t))

typedef enum {
	ADC_RES_8,
	ADC_RES_10,
	ADC_RES_12,
	ADC_RES_14,
	ADC_RES_16,
} packet_adc_res_t;

typedef enum {
	PACKET_TYPE_TIMER_DELTA,
	PACKET_TYPE_ADC_VALUES,
	PACKET_TYPE_CONFIG,
	PACKET_TYPE_DEVICE_INFO,

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
	config_t config;
	uint32_t crc;
} packet_config_t;

typedef struct {
	uint32_t name;
	uint32_t clock;
	uint8_t adc_res;
	uint8_t adc_num;
	uint8_t reserved[2];
} device_conf_t;

typedef struct {
	packet_header_t header;
	device_conf_t device;
	uint32_t crc;
} packet_device_info_t;

typedef struct {
	packet_header_t header;
	uint32_t crc;
} packet_simple_t;