#include "packet.h"
#include "crc.h"
#include "tusb.h"

void packet_timer_delta_init(packet_timer_delta_t* packet) {
	packet->header.magic = PACKET_HEADER_MAGIC;
	packet->header.length = sizeof(packet_timer_delta_t);
	packet->header.type = PACKET_TYPE_TIMER_DELTA;
}

void packet_timer_delta_fill(packet_timer_delta_t* packet, const uint32_t* data, uint32_t *last_capture) {
	uint32_t last = *last_capture;
	for(size_t i = 0; i < DMA_HALF_BUFFER_SIZE; i++) {
		uint32_t current = data[i];
		uint32_t delta = current - last;

		last = current;
		packet->deltas[i] = delta;
	}
	packet->crc = HAL_CRC_Calculate(&hcrc, (const uint32_t*) packet,
			PACKET_RAW_SIZE(packet_timer_delta_t));
	*last_capture = last;
}

void packet_config_init(packet_config_t *packet) {
	packet->header.magic = PACKET_HEADER_MAGIC;
	packet->header.length = sizeof(packet_timer_delta_t);
	packet->header.type = PACKET_TYPE_CONFIG;

	packet->config = g_config;
	packet->crc = HAL_CRC_Calculate(&hcrc, (const uint32_t*) packet,
			PACKET_RAW_SIZE(packet_config_t));
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

void tud_vendor_rx_cb(uint8_t itf, const uint8_t *buffer, uint32_t bufsize) {
	(void)buffer;
	(void)bufsize;

	while(tud_vendor_available()) {
		uint8_t buff[64];
		uint32_t count = tud_vendor_n_read(itf, buff, sizeof(buff));

		for (uint32_t i = 0; i < count;) {
			switch(s_packet_process) {
			case PACKET_PROCESS_WAIT_MAGIC:
				if (i == (count - 2)) break;
				if (*(uint16_t*)&buff[i] == PACKET_HEADER_MAGIC) {
					s_packet_process = PACKET_PROCESS_READ_HEADER;
					s_packet_read_length = 0;
				}
				i++;
				break;
			case PACKET_PROCESS_READ_HEADER:
				s_packet_read_buffer[s_packet_read_length++] = buff[i++];
				if (s_packet_read_length >= sizeof(packet_header_t)) {
					const packet_header_t* header = (const packet_header_t*) s_packet_read_buffer;

					s_packet_read_length 		= 0;
					s_packet_expected_length 	= header->length - sizeof(packet_header_t);
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
}

void packet_init(packet_callback cb) {
	s_packet_callback = cb;
}
