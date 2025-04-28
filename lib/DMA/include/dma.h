#pragma once

/*!
    \file   dma.h
    \brief  DMA driver interface

    This header defines the interface for configurating DMA to 
    transfer ADC data continously in circular mode.
*/

#include "gd32vf103.h"
#include <stdint.h>

#define NUM_OF_ADC_CHANNELS     5
#define PERIOD_LENGTH           20  // 20ms = 1 period
#define BUFFER_LENGTH           NUM_OF_ADC_CHANNELS * PERIOD_LENGTH // 100

/** DMA interrupt handler triggered and buffer ready control */
volatile uint8_t buffer_ready;

/** DMA buffer for ADC values (defined int adc.h) */
extern uint16_t adc_value[];

/** External buffer that stores 100 values */
uint16_t dma_buffer[BUFFER_LENGTH];

uint16_t adc0_ch1_values[PERIOD_LENGTH];
uint16_t adc0_ch2_values[PERIOD_LENGTH];
uint16_t adc0_ch3_values[PERIOD_LENGTH];
uint16_t adc0_ch4_values[PERIOD_LENGTH];
uint16_t adc0_ch5_values[PERIOD_LENGTH];

/** Each ADC channel has respective array of value that get copied
 *  from the DMA to an array by the interrupt handler. This provides as 
 *  both a collection and a lookup table for those buffers, meant to be global */
uint16_t* adc_channels[5];

/* Initialize the DMA with full transfer interrupt */
void dma_ftf_init(void);
