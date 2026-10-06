#include "config.h"
#include "application.h"
#include "crc.h"

#define CONFIG_FLASH_ADDR  0x0807F800UL

config_t g_config __attribute((aligned(0x40)));

static void _config_init_default(void) {
	g_config.magic = CONFIG_MAGIC;
	g_config.encoder_resolution = 600;
	g_config.encoder_mode = ENCODER_CAPTURE_XOR;
	g_config.encoder_channel = ENCODER_CHANNEL_A;

	config_save();
}

void config_init(void) {
	static const config_t* const flash_config = (const config_t*)CONFIG_FLASH_ADDR;
	g_config = *flash_config;

	if (g_config.magic != CONFIG_MAGIC) {
		_config_init_default();
		return;
	}

	if (g_config.crc != HAL_CRC_Calculate(&hcrc, (const uint32_t *) &g_config, sizeof(g_config) - sizeof(uint32_t))) {
		_config_init_default();
		return;
	}
}

void config_save(void) {
	g_config.crc = HAL_CRC_Calculate(&hcrc, (const uint32_t *) &g_config, sizeof(g_config) - sizeof(uint32_t));

	HAL_FLASH_Unlock();

	FLASH_EraseInitTypeDef erase = {0};

	erase.TypeErase = FLASH_TYPEERASE_PAGES;
	erase.Banks 	= FLASH_BANK_2;
	erase.Page		= 127;
	erase.NbPages 	= 1;

	uint32_t error = 0;
	HAL_StatusTypeDef ret = HAL_FLASHEx_Erase(&erase, &error);

	if (ret != HAL_OK) {
		HAL_FLASH_Lock();
		return;
	}

	const uint64_t *src = (const uint64_t*) &g_config;
	for(int i = 0; i < sizeof(g_config)/sizeof(uint64_t); i++) {
		ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, CONFIG_FLASH_ADDR + i * 8, src[i]);

		if (ret != HAL_OK) {
			HAL_FLASH_Lock();
			return;
		}
	}

	HAL_FLASH_Lock();
}
