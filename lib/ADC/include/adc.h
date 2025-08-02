#pragma once

/*!
    \file   adc.h
    \brief  ADC driver interface

    This header defines the interface for initializing the ADC in scan mode 
    with DMA and TIMER2 support. 

    It also defines the ADC channels being used
*/

#include "gd32vf103.h"
#include "usb_delay.h"
#include <stdint.h>
#include "dma.h"

#define ADC_SIZE              5
#define ADC_GPIO_PINS       ( GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 )

/* Initialize ADC functionality and RCU with scan mode, continous disable */
void adc_dma_init(void);
