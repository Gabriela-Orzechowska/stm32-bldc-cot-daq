#include "application.h"
#include "dma.h"
#include "tim.h"
#include "crc.h"
#include "tusb.h"
#include "stm32g4xx.h"
#include "packet.h"

static volatile bool s_buffer_ready_half = {0};
static volatile bool s_buffer_ready_full = {0};
static uint32_t s_buffer_timer[DMA_BUFFER_SIZE] = {0};
static uint32_t s_last_capture = {0};
static packet_timer_delta_t s_packet_timer = {0};

static void application_init(void);
static void application_loop(void);

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

	packet_timer_delta_init(&s_packet_timer);
}

static void timer_init(encoder_capture_mode_t mode, encoder_channel_t channel);

static void application_init(void) {
	interrupt_init();
	usb_init();
	timer_init(ENCODER_CAPTURE_XOR, ENCODER_CHANNEL_A);
}

static void application_loop(void) {
	// Process timer packets
	bool has_data = false;
	if (s_buffer_ready_half) {
		packet_timer_delta_fill(&s_packet_timer, s_buffer_timer, &s_last_capture);
		s_buffer_ready_half = false;
		has_data = true;
	}
	if (s_buffer_ready_full) {
		packet_timer_delta_fill(&s_packet_timer, &s_buffer_timer[DMA_HALF_BUFFER_SIZE], &s_last_capture);
		s_buffer_ready_full = false;
		has_data = true;
	}

	if (has_data) {
		tud_vendor_write(&s_packet_timer, sizeof(packet_timer_delta_t));
		tud_vendor_write_flush();
	}

	tud_task();
}


static void interrupt_dma_complete_half(TIM_HandleTypeDef* htim) {
	s_buffer_ready_half = true;
}
static void interrupt_dma_complete_full(TIM_HandleTypeDef* htim) {
	s_buffer_ready_full = true;
}
static void interrupt_init(void) {
	HAL_TIM_RegisterCallback(&htim2, HAL_TIM_IC_CAPTURE_HALF_CB_ID, interrupt_dma_complete_half);
	HAL_TIM_RegisterCallback(&htim2, HAL_TIM_IC_CAPTURE_CB_ID, interrupt_dma_complete_full);
}

static void timer_init(encoder_capture_mode_t mode, encoder_channel_t channel) {
	HAL_TIM_IC_Stop_DMA(&htim2, TIM_CHANNEL_1);

	CLEAR_BIT(TIM2->CCER, TIM_CCER_CC1E);
	CLEAR_BIT(TIM2->CR2, TIM_CR2_TI1S);

	switch(mode) {
	case ENCODER_CAPTURE_ONE:
	case ENCODER_CAPTURE_BOTH:
		if (channel == ENCODER_CHANNEL_A)
			MODIFY_REG(TIM2->CCMR1, TIM_CCMR1_CC1S, TIM_CCMR1_CC1S_0);
		else
			MODIFY_REG(TIM2->CCMR1, TIM_CCMR1_CC1S, TIM_CCMR1_CC1S_1);
		break;
	case ENCODER_CAPTURE_XOR:
		MODIFY_REG(TIM2->CCMR1, TIM_CCMR1_CC1S, TIM_CCMR1_CC1S_0);
		SET_BIT(TIM2->CR2, TIM_CR2_TI1S);
		break;
	default:
		return;
	}

	if (mode == ENCODER_CAPTURE_ONE) {
		CLEAR_BIT(TIM2->CCER, TIM_CCER_CC1P | TIM_CCER_CC1NP);
	}
	else {
		SET_BIT(TIM2->CCER, TIM_CCER_CC1P | TIM_CCER_CC1NP);
	}

	WRITE_REG(TIM2->SR, ~TIM_SR_CC1IF);
	SET_BIT(TIM2->CCER, TIM_CCER_CC1E);
	HAL_TIM_IC_Start_DMA(&htim2, TIM_CHANNEL_1, s_buffer_timer, DMA_BUFFER_SIZE);
}
