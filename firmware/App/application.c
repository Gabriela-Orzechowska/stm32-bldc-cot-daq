#include "application.h"
#include "dma.h"
#include "tim.h"
#include "adc.h"
#include "crc.h"
#include "tusb.h"
#include "stm32h7xx.h"
#include "packet.h"
#include "config.h"
#include "lwip.h"
#include "tcp.h"

static volatile bool s_buffer_ready_half = {0};
static volatile bool s_buffer_ready_full = {0};
static volatile bool s_adc_ready_half = 0;
static volatile bool s_adc_ready_full = 0;

ALIGN_NO_CACHE_D2 static uint32_t s_buffer_timer[DMA_BUFFER_SIZE] = {0};
ALIGN_NO_CACHE_D2 static uint8_t s_packet_timer[PACKET_TIMER_MINIMAL_SPACE];
ALIGN_NO_CACHE_D3 static uint16_t s_adc_samples[DMA_BUFFER_SIZE * ADC_CHANNEL_COUNT] = {0};

static uint32_t s_last_capture = {0};


static void _application_init(void);
static void _application_loop(void);

void application_entry(void) {
	_application_init();
	for(;;) _application_loop();
}
static void _packet_receive(packet_type_t type, const void* data, size_t size);
static void _interrupt_init(void);

static void _timer_init(encoder_capture_mode_t mode, encoder_channel_t channel);

static void _application_client_connect(void) {
	packet_device_info_t info;
	packet_device_info_init(&info);

	server_send_data(&info, sizeof(info));

	packet_config_t config;
	packet_config_init(&config);

	server_send_data(&config, sizeof(config));
}
static void _application_client_data_receive(const void* data, size_t size) {
	packet_process_data(data, size);
}

static void _application_init(void) {
	_interrupt_init();
	packet_init(_packet_receive);


	HAL_ADC_Start_DMA(&hadc3, (uint32_t*) s_adc_samples, (DMA_BUFFER_SIZE * ADC_CHANNEL_COUNT));
	HAL_TIM_Base_Start(&htim3);

	config_init();
	_timer_init(g_config.encoder_mode, g_config.encoder_channel);

	server_init();
	server_set_on_connect_callback(_application_client_connect);
	server_set_recv_callback(_application_client_data_receive);
}


static void _application_loop(void) {
	MX_LWIP_Process();

	if (s_buffer_ready_half) {
		uint16_t packet_size = packet_timer_delta_fill((uint16_t*) s_packet_timer, s_buffer_timer, &s_last_capture);
		s_buffer_ready_half = false;
		server_send_data(s_packet_timer, packet_size);

	}
	else if (s_buffer_ready_full) {
		uint16_t packet_size = packet_timer_delta_fill((uint16_t*)s_packet_timer, &s_buffer_timer[DMA_HALF_BUFFER_SIZE], &s_last_capture);
		s_buffer_ready_full = false;
		server_send_data(s_packet_timer, packet_size);
	} else if (s_adc_ready_half) {
		const size_t half_buffer =  (DMA_HALF_BUFFER_SIZE * ADC_CHANNEL_COUNT);
		packet_header_t header;
		packet_adc_init(&header);

		uint32_t crc = HAL_CRC_Calculate(&hcrc, (uint32_t*) &header, sizeof(header));
		crc = HAL_CRC_Accumulate(&hcrc, (uint32_t*)s_adc_samples, half_buffer * sizeof(uint16_t));

		server_send_data(&header, sizeof(header));
		server_send_data(s_adc_samples, half_buffer * sizeof(uint16_t));
		server_send_data(&crc, sizeof(crc));
		s_adc_ready_half = false;
	} else if (s_adc_ready_full) {
		const size_t half_buffer =  (DMA_HALF_BUFFER_SIZE * ADC_CHANNEL_COUNT);
		packet_header_t header;
		packet_adc_init(&header);

		uint32_t crc = HAL_CRC_Calculate(&hcrc, (uint32_t*)&header, sizeof(header));
		crc = HAL_CRC_Accumulate(&hcrc, (uint32_t*)&s_adc_samples[half_buffer], half_buffer * sizeof(uint16_t));

		server_send_data(&header, sizeof(header));
		server_send_data(&s_adc_samples[half_buffer], half_buffer * sizeof(uint16_t));
		server_send_data(&crc, sizeof(crc));
		s_adc_ready_full = false;
	}
}

static void _packet_receive(packet_type_t type, const void* data, size_t size) {
	if (type == PACKET_TYPE_CONFIG) {
		if (size < sizeof(packet_config_t)) return;
		const packet_config_t* packet = (const packet_config_t*) data;

		uint32_t calculatedCRC = HAL_CRC_Calculate(&hcrc, (uint32_t*) packet, PACKET_RAW_SIZE(packet_config_t));
		if (packet->crc != calculatedCRC)
			return;

		g_config = packet->config;
		config_save();

		NVIC_SystemReset();
	}
}

static void _interrupt_dma_complete_half(TIM_HandleTypeDef* htim) {
	s_buffer_ready_half = true;
}
static void _interrupt_dma_complete_full(TIM_HandleTypeDef* htim) {
	s_buffer_ready_full = true;
}

static void _interrupt_dma_adc_half(ADC_HandleTypeDef* hadc) {
	s_adc_ready_half = true;
}

static void _interrupt_dma_adc_full(ADC_HandleTypeDef* hadc) {
	s_adc_ready_half = false;
}


static void _interrupt_init(void) {
	HAL_TIM_RegisterCallback(&htim2, HAL_TIM_IC_CAPTURE_HALF_CB_ID, _interrupt_dma_complete_half);
	HAL_TIM_RegisterCallback(&htim2, HAL_TIM_IC_CAPTURE_CB_ID, _interrupt_dma_complete_full);

	HAL_ADC_RegisterCallback(&hadc3, HAL_ADC_CONVERSION_HALF_CB_ID, _interrupt_dma_adc_half);
	HAL_ADC_RegisterCallback(&hadc3, HAL_ADC_CONVERSION_COMPLETE_CB_ID, _interrupt_dma_adc_full);
}

static void _timer_init(encoder_capture_mode_t mode, encoder_channel_t channel) {
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
