#pragma once

/*!
    \file   adc.h
    \brief  ADC driver interface

    This header defines the interface for initializing the ADC in scan mode 
    with DMA support with continous sampling enables. 

    It also defines number of ADC channels being used and provides
    access to the samples ADC values.
*/

#include "gd32vf103.h"
#include "usb_delay.h"
#include <stdint.h>

#define ADC_NUMBER          5 // total amount of pins
#define ADC_GPIO_PINS       ( GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 )

uint16_t adc_value[ADC_NUMBER];

/* Initialize ADC functionality and clocks with scan mode, continous enable */
void adc_dma_init(void);
