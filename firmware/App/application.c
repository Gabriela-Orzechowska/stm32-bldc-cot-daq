#include "application.h"
#include "dma.h"
#include "tim.h"

uint32_t timer_buffer[ENCODER_CHANNEL_COUNT][DMA_BUFFER_SIZE] = {0};
uint32_t last_capture_tick[ENCODER_CHANNEL_COUNT] = {0};
volatile uint8_t buffer_half_ready[ENCODER_CHANNEL_COUNT] = {0};
volatile uint8_t buffer_full_ready[ENCODER_CHANNEL_COUNT] = {0};

static void application_init(void);
static void application_loop(void);

void application_entry(void) {
	application_init();
	for(;;) application_loop();
}

static void interrupt_init(void);
void application_init(void) {
	interrupt_init();
}

void application_loop(void) {

}


static void interrupt_dma_complete_half(TIM_HandleTypeDef* htim) {
	if (htim->Instance == TIM2) {
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) buffer_half_ready[ENCODER_A_RISING] = 1;
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) buffer_half_ready[ENCODER_A_FALLING] = 1;
	} else if (htim->Instance == TIM5) {
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) buffer_half_ready[ENCODER_B_RISING] = 1;
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) buffer_half_ready[ENCODER_B_FALLING] = 1;
	}
}
static void interrupt_dma_complete_full(TIM_HandleTypeDef* htim) {
	if (htim->Instance == TIM2) {
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) buffer_full_ready[ENCODER_A_RISING] = 1;
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) buffer_full_ready[ENCODER_A_FALLING] = 1;
	} else if (htim->Instance == TIM5) {
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) buffer_full_ready[ENCODER_B_RISING] = 1;
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) buffer_full_ready[ENCODER_B_FALLING] = 1;
	}
}
static void interrupt_init(void) {
	HAL_TIM_RegisterCallback(&htim2, HAL_TIM_IC_CAPTURE_HALF_CB_ID, interrupt_dma_complete_half);
	HAL_TIM_RegisterCallback(&htim2, HAL_TIM_IC_CAPTURE_CB_ID, interrupt_dma_complete_full);

	HAL_TIM_RegisterCallback(&htim5, HAL_TIM_IC_CAPTURE_HALF_CB_ID, interrupt_dma_complete_half);
	HAL_TIM_RegisterCallback(&htim5, HAL_TIM_IC_CAPTURE_CB_ID, interrupt_dma_complete_full);
}
