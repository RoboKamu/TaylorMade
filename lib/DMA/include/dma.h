#pragma once

/*!
    \file   dma.h
    \brief  DMA driver interface

    This header defines the interface for configurating DMA to 
    transfer ADC data continously in circular mode.
*/

#include "gd32vf103.h"
#include <stdint.h>

/** External buffer for storing ADC values (defined int adc.h) */
extern uint16_t adc_value[];

/* configure DMA clock */
void dma_config(void);
