#pragma once
#include "common.h"

#define CONFIG_MAGIC 0x434F4E46UL

typedef PACKED_STRUCT() {
	float min_val;
	float max_val;
	char unit[3];
	uint8_t enabled;
} config_adc_setup_t;

typedef PACKED_STRUCT() {
	uint32_t magic;

	uint8_t encoder_mode;
	uint8_t encoder_channel;
	uint16_t encoder_resolution;

	uint8_t adc_div;
	uint8_t interlace;
	uint8_t reserved[2];

	config_adc_setup_t adc_config[ADC_CHANNEL_COUNT];

	uint32_t crc;
} config_t;

extern config_t g_config;

void config_init(void);
void config_save(void);
