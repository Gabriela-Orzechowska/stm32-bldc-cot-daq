#pragma once
#include "types.h"
#include "common.h"

typedef enum
{
    ENCODER_CHANNEL_A,
    ENCODER_CHANNEL_B
} encoder_channel_t;

typedef enum
{
    ENCODER_CAPTURE_ONE,
    ENCODER_CAPTURE_BOTH,
    ENCODER_CAPTURE_XOR,
} encoder_capture_mode_t;

void application_entry(void);
