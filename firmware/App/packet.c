#include "packet.h"
#include "crc.h"
#include "tusb.h"

uint16_t packet_timer_delta_fill(uint16_t* packet, const uint32_t* data, uint32_t *last_capture) {
	uint16_t packet_len = sizeof(packet_header_t);

	packet_header_t* header = (packet_header_t*) packet;
	header->magic	= PACKET_HEADER_MAGIC;
	// header->length = to be computed
	header->type	= PACKET_TYPE_TIMER_DELTA;

	uint16_t* deltas = packet + (sizeof(packet_header_t) / sizeof(uint16_t));
	uint16_t* delta_start = deltas;

	uint32_t last_value = *last_capture;

	for (int i = 0; i < DMA_HALF_BUFFER_SIZE; i++) {
		uint32_t value = data[i];
		uint32_t difference = value - last_value;

		if (difference >= 0xFFFF) {
			*deltas++ = 0xFFFF;
			*deltas++ = (difference >> 16) & 0xFFFF;

		}
		*deltas++ = difference & 0xFFFF;

		last_value = value;
	}
	*last_capture = last_value;

	packet_len += (deltas - delta_start) * sizeof(*deltas);
	header->length = packet_len + sizeof(uint32_t);

	uint32_t crc = HAL_CRC_Calculate(&hcrc, (uint32_t*) packet, packet_len);
	*deltas++ = (crc >> 0) & 0xFFFF;
	*deltas++ = (crc >> 16) & 0xFFFF;

	return packet_len + sizeof(uint32_t);
}

void packet_config_init(packet_config_t *packet) {
	memset(packet, 0, sizeof(packet_config_t));

	packet->header.magic = PACKET_HEADER_MAGIC;
	packet->header.length = sizeof(packet_config_t);
	packet->header.type = PACKET_TYPE_CONFIG;

	packet->config = g_config;
	packet->crc = HAL_CRC_Calculate(&hcrc, (uint32_t*) packet,
			PACKET_RAW_SIZE(packet_config_t));
}

void packet_device_info_init(packet_device_info_t *packet) {
	memset(packet, 0, sizeof(packet_device_info_t));

	packet->header.magic = PACKET_HEADER_MAGIC;
	packet->header.length = sizeof(packet_device_info_t);
	packet->header.type = PACKET_TYPE_DEVICE_INFO;

	packet->device_name = 'H753';
	packet->device_clock = HAL_RCC_GetPCLK2Freq();
	if ((RCC->D2CFGR & RCC_D2CFGR_D2PPRE2) != RCC_D2CFGR_D2PPRE2_DIV1) {
		packet->device_clock *= 2U;
	}

	packet->crc = HAL_CRC_Calculate(&hcrc, (uint32_t*) packet,
			PACKET_RAW_SIZE(packet_device_info_t));
}

typedef enum {
	PACKET_PROCESS_WAIT_MAGIC,
	PACKET_PROCESS_READ_HEADER,
	PACKET_PROCESS_READ_PAYLOAD,
} _packet_process_t;

static packet_callback s_packet_callback = NULL;
static int s_packet_read_length = 0;
static int s_packet_expected_length = 0;
static _packet_process_t s_packet_process = PACKET_PROCESS_WAIT_MAGIC;
static uint16_t s_packet_type = 0;
static uint8_t s_packet_read_buffer[256];

void packet_process_data(const void *buffer, size_t size) {
	const uint8_t* buff = (const uint8_t*) buffer;
	size_t count = size;

	for (size_t i = 0; i < count;) {
		switch(s_packet_process) {
		case PACKET_PROCESS_WAIT_MAGIC:
			if (i >= (count - 2)) return;
			if (*(uint16_t*)&buff[i] == PACKET_HEADER_MAGIC) {
				s_packet_process = PACKET_PROCESS_READ_HEADER;
				s_packet_read_length = 0;
			} else i++;
		case PACKET_PROCESS_READ_HEADER:
			s_packet_read_buffer[s_packet_read_length++] = buff[i++];
			if (s_packet_read_length >= sizeof(packet_header_t)) {
				const packet_header_t* header = (const packet_header_t*) s_packet_read_buffer;

				s_packet_expected_length 	= header->length;
				s_packet_type 				= header->type;
				s_packet_process 			= PACKET_PROCESS_READ_PAYLOAD;

				if (s_packet_type >= PACKET_TYPE_COUNT
						|| s_packet_expected_length > sizeof(s_packet_read_buffer)) {
					s_packet_process = PACKET_PROCESS_WAIT_MAGIC;
				}
			}
			break;
		case PACKET_PROCESS_READ_PAYLOAD:
			s_packet_read_buffer[s_packet_read_length++] = buff[i++];
			if (s_packet_read_length >= s_packet_expected_length) {
				if (s_packet_callback) s_packet_callback(s_packet_type,
						s_packet_read_buffer, s_packet_expected_length);
				s_packet_process = PACKET_PROCESS_WAIT_MAGIC;
			}
			break;

		default:
			i++;
		}
	}
}

void packet_init(packet_callback cb) {
	s_packet_callback = cb;
}
