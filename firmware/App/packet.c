#include "packet.h"
#include "crc.h"

void packet_timer_delta_init(packet_timer_delta_t* packet) {
	packet->header.magic = 0x55AA;
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
	packet->crc = 0;
	packet->crc = HAL_CRC_Calculate(&hcrc, (const uint32_t*) packet, sizeof(packet_timer_delta_t));
	*last_capture = last;
}
