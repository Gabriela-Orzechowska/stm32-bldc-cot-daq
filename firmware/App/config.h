#pragma once
#include "common.h"

#define CONFIG_MAGIC 0x434F4E46UL

typedef struct {
	uint32_t magic;

	uint8_t encoder_mode;
	uint8_t encoder_channel;
	uint16_t encoder_resolution;

	uint8_t reserved[4];

	uint32_t crc;
} config_t;

extern config_t g_config;

void config_init(void);
void config_save(void);
