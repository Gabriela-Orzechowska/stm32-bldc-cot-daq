#include "application.h"
#include "dma.h"
#include "tim.h"
#include "tusb.h"

static volatile uint8_t s_buffer_ready_half[ENCODER_CHANNEL_COUNT] = {0};
static volatile uint8_t s_buffer_ready_full[ENCODER_CHANNEL_COUNT] = {0};
static uint32_t s_buffer_timer[ENCODER_CHANNEL_COUNT][DMA_BUFFER_SIZE] = {0};
static uint32_t s_last_capture[ENCODER_CHANNEL_COUNT] = {0};


static payload_timer_delta_t s_packet_timer[ENCODER_CHANNEL_COUNT] = {0};

static void application_init(void);
static void application_loop(void);
static void application_process_buffer(uint32_t *buffer, uint32_t* out_buffer, encoder_channel_t channel, size_t size);

void application_entry(void) {
	application_init();
	for(;;) application_loop();
}

static void interrupt_init(void);
static void application_init(void) {
	interrupt_init();

	// Initialize USB
	tusb_rhport_init_t dev_init = {
		  .role = TUSB_ROLE_DEVICE,
		  .speed = TUSB_SPEED_AUTO,
	};
	tusb_init(0, &dev_init);

	// Initialize PC Packets
	for(int i = 0; i < ENCODER_CHANNEL_COUNT; i++) {
		s_packet_timer[i].header.header = 0x55AA;
		s_packet_timer[i].header.length = sizeof(s_packet_timer[i].deltas);
	}
}

static void application_loop(void) {
	// Process timer packets
	for(int i = 0; i < ENCODER_CHANNEL_COUNT; i++) {
		uint8_t has_data = 0;
		if (s_buffer_ready_half[i] == 1) {
			application_process_buffer(s_buffer_timer[i], s_packet_timer[i].deltas, i, DMA_HALF_BUFFER_SIZE);
			s_buffer_ready_half[i] = 0;
			has_data = 1;
		}
		if (s_buffer_ready_full[i] == 1) {
			application_process_buffer(&s_buffer_timer[i][DMA_HALF_BUFFER_SIZE], s_packet_timer[i].deltas, i, DMA_HALF_BUFFER_SIZE);
			s_buffer_ready_full[i] = 0;
			has_data = 1;
		}

		if (has_data == 1) {
			tud_vendor_write(&s_packet_timer[i], sizeof(payload_timer_delta_t));
			tud_vendor_write_flush();
		}
	}

	tud_task();
}

static void application_process_buffer(uint32_t* buffer, uint32_t* out_buffer, encoder_channel_t channel, size_t size) {
	uint32_t last_capture = s_last_capture[channel];

	for(size_t i = 0; i < size; i++) {
		uint32_t current = buffer[i];
		uint32_t delta = current - last_capture;

		last_capture = current;
		out_buffer[i] = delta;
	}

	s_last_capture[channel] = last_capture;
}


static void interrupt_dma_complete_half(TIM_HandleTypeDef* htim) {
	if (htim->Instance == TIM2) {
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) s_buffer_ready_half[ENCODER_A_RISING] = 1;
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) s_buffer_ready_half[ENCODER_A_FALLING] = 1;
	} else if (htim->Instance == TIM5) {
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) s_buffer_ready_half[ENCODER_B_RISING] = 1;
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) s_buffer_ready_half[ENCODER_B_FALLING] = 1;
	}
}
static void interrupt_dma_complete_full(TIM_HandleTypeDef* htim) {
	if (htim->Instance == TIM2) {
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) s_buffer_ready_full[ENCODER_A_RISING] = 1;
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) s_buffer_ready_full[ENCODER_A_FALLING] = 1;
	} else if (htim->Instance == TIM5) {
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) s_buffer_ready_full[ENCODER_B_RISING] = 1;
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) s_buffer_ready_full[ENCODER_B_FALLING] = 1;
	}
}
static void interrupt_init(void) {
	HAL_TIM_RegisterCallback(&htim2, HAL_TIM_IC_CAPTURE_HALF_CB_ID, interrupt_dma_complete_half);
	HAL_TIM_RegisterCallback(&htim2, HAL_TIM_IC_CAPTURE_CB_ID, interrupt_dma_complete_full);

	HAL_TIM_RegisterCallback(&htim5, HAL_TIM_IC_CAPTURE_HALF_CB_ID, interrupt_dma_complete_half);
	HAL_TIM_RegisterCallback(&htim5, HAL_TIM_IC_CAPTURE_CB_ID, interrupt_dma_complete_full);
}
