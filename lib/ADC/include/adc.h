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

#define ADC_NUMBER          5 // total amount of pins
#define ADC_GPIO_PINS       ( GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 )

uint16_t adc_value[ADC_NUMBER];

/* Initialize ADC functionality and RCU with scan mode, continous disable */
void adc_dma_init(void);
