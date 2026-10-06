#include "application.h"
#include "dma.h"
#include "tim.h"
#include "crc.h"
#include "tusb.h"

static volatile bool s_buffer_ready_half[ENCODER_CHANNEL_COUNT] = {0};
static volatile bool s_buffer_ready_full[ENCODER_CHANNEL_COUNT] = {0};
static uint32_t s_buffer_timer[ENCODER_CHANNEL_COUNT][DMA_BUFFER_SIZE] = {0};
static uint32_t s_last_capture[ENCODER_CHANNEL_COUNT] = {0};

static payload_timer_delta_t s_packet_timer[ENCODER_CHANNEL_COUNT] = {0};

static void application_init(void);
static void application_loop(void);
static void payload_timer_process(payload_timer_delta_t* packet, const uint32_t* buffer, encoder_channel_t channel);

void application_entry(void) {
	application_init();
	for(;;) application_loop();
}

static void interrupt_init(void);
static void usb_init(void) {
	tusb_rhport_init_t dev_init = {
			  .role = TUSB_ROLE_DEVICE,
			  .speed = TUSB_SPEED_FULL,
	};
	tusb_init(0, &dev_init);

	// Initialize PC Packets
	for(int i = 0; i < ENCODER_CHANNEL_COUNT; i++) {
		s_packet_timer[i].header.magic 		= 0x55AA;
		s_packet_timer[i].header.length 	= sizeof(s_packet_timer[i].deltas);
		s_packet_timer[i].header.type 		= PACKET_TYPE_ENCODER_DELTA;
		s_packet_timer[i].header.subtype	= i;
	}
}

static void timer_init(void) {
	HAL_TIM_IC_Start_DMA(&htim2, TIM_CHANNEL_1, s_buffer_timer[0], DMA_BUFFER_SIZE);
	HAL_TIM_IC_Start_DMA(&htim2, TIM_CHANNEL_2, s_buffer_timer[1], DMA_BUFFER_SIZE);
	HAL_TIM_IC_Start_DMA(&htim5, TIM_CHANNEL_1, s_buffer_timer[2], DMA_BUFFER_SIZE);
	HAL_TIM_IC_Start_DMA(&htim5, TIM_CHANNEL_2, s_buffer_timer[3], DMA_BUFFER_SIZE);
}

static void application_init(void) {
	interrupt_init();
	usb_init();
	timer_init();
}

static void application_loop(void) {
	// Process timer packets
	for(int i = 0; i < 1; i++) {
		bool has_data = false;
		if (s_buffer_ready_half[i]) {
			payload_timer_process(&s_packet_timer[i], s_buffer_timer[i], i);
			s_buffer_ready_half[i] = false;
			has_data = true;
		}
		if (s_buffer_ready_full[i]) {
			payload_timer_process(&s_packet_timer[i], &s_buffer_timer[i][DMA_HALF_BUFFER_SIZE], i);
			s_buffer_ready_full[i] = false;
			has_data = true;
		}

		if (has_data) {
			tud_vendor_write(&s_packet_timer[i], sizeof(payload_timer_delta_t));
			tud_vendor_write_flush();
			tud_task();
		}
	}

	tud_task();
}

static void payload_timer_process(payload_timer_delta_t* packet, const uint32_t* buffer, encoder_channel_t channel) {
	uint32_t last_capture = s_last_capture[channel];

	for(size_t i = 0; i < DMA_HALF_BUFFER_SIZE; i++) {
		uint32_t current = buffer[i];
		uint32_t delta = current - last_capture;

		last_capture = current;
		packet->deltas[i] = delta;
	}
	packet->crc = 0;
	packet->crc = HAL_CRC_Calculate(&hcrc, (const uint32_t*) packet, sizeof(payload_timer_delta_t));

	s_last_capture[channel] = last_capture;
}


static void interrupt_dma_complete_half(TIM_HandleTypeDef* htim) {
	if (htim->Instance == TIM2) {
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) s_buffer_ready_half[ENCODER_A_RISING] = true;
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) s_buffer_ready_half[ENCODER_A_FALLING] = true;
	} else if (htim->Instance == TIM5) {
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) s_buffer_ready_half[ENCODER_B_RISING] = true;
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) s_buffer_ready_half[ENCODER_B_FALLING] = true;
	}
}
static void interrupt_dma_complete_full(TIM_HandleTypeDef* htim) {
	if (htim->Instance == TIM2) {
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) s_buffer_ready_full[ENCODER_A_RISING] = true;
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) s_buffer_ready_full[ENCODER_A_FALLING] = true;
	} else if (htim->Instance == TIM5) {
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) s_buffer_ready_full[ENCODER_B_RISING] = true;
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) s_buffer_ready_full[ENCODER_B_FALLING] = true;
	}
}
static void interrupt_init(void) {
	HAL_TIM_RegisterCallback(&htim2, HAL_TIM_IC_CAPTURE_HALF_CB_ID, interrupt_dma_complete_half);
	HAL_TIM_RegisterCallback(&htim2, HAL_TIM_IC_CAPTURE_CB_ID, interrupt_dma_complete_full);

	HAL_TIM_RegisterCallback(&htim5, HAL_TIM_IC_CAPTURE_HALF_CB_ID, interrupt_dma_complete_half);
	HAL_TIM_RegisterCallback(&htim5, HAL_TIM_IC_CAPTURE_CB_ID, interrupt_dma_complete_full);
}
