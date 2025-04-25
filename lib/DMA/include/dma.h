
#pragma once

#include "gd32vf103.h"
#include <stdint.h>

extern uint16_t adc_value[];    // based on adc.h 

/* configure DMA clock */
void dma_config(void);
