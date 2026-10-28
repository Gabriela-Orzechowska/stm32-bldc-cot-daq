#include "config.h"
#include "application.h"
#include "crc.h"
#include <string.h>

#define CONFIG_FLASH_ADDR  	0x081E0000UL
#define CONFIG_FLASH_BANK	FLASH_BANK_2
#define CONFIG_FLASH_SECTOR FLASH_SECTOR_7

config_t g_config __attribute((aligned(0x40)));

static void _config_init_default(void) {
	g_config.magic = CONFIG_MAGIC;
	g_config.encoder_resolution = 600;
	g_config.encoder_mode = ENCODER_CAPTURE_ONE;
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

	if (g_config.crc != HAL_CRC_Calculate(&hcrc, (uint32_t *) &g_config, sizeof(g_config) - sizeof(uint32_t))) {
		_config_init_default();
		return;
	}
}

void config_save(void) {
	static uint8_t buffer[0x20];

	g_config.crc = HAL_CRC_Calculate(&hcrc, (uint32_t *) &g_config, sizeof(g_config) - sizeof(uint32_t));
	HAL_FLASH_Unlock();

	FLASH_EraseInitTypeDef erase = {0};

	erase.TypeErase 	= FLASH_TYPEERASE_SECTORS;
	erase.Banks 		= CONFIG_FLASH_BANK;
	erase.Sector		= CONFIG_FLASH_SECTOR;
	erase.NbSectors 	= 1;
	erase.VoltageRange 	= FLASH_VOLTAGE_RANGE_3;

	uint32_t error = 0;
	HAL_StatusTypeDef ret = HAL_FLASHEx_Erase(&erase, &error);

	if (ret != HAL_OK) {
		HAL_FLASH_Lock();
		return;
	}


	const size_t padded_size = (sizeof(config_t) + 0x1F) & ~0x1F;
	for(int i = 0; i < padded_size; i += 0x20) {
		memset(buffer, 0xFF, sizeof(buffer));

		if (i < sizeof(config_t)) {
			uint32_t copy_size = sizeof(config_t) - i;
			copy_size = copy_size < 0x20 ? copy_size : 0x20;
			memcpy(buffer, (const uint8_t*)&g_config + i, copy_size);
		}

		ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, CONFIG_FLASH_ADDR + i, (uint32_t)buffer);

		if (ret != HAL_OK) {
			HAL_FLASH_Lock();
			return;
		}
	}

	HAL_FLASH_Lock();

}
