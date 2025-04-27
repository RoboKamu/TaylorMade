/*!
    \file   dma.c
    \brief  DMA driver implementation

    The goal is to have DMA like a circular buffer, continously storing ADC data.
    Once a defined amount of transers (signal periods) are completed, an interrupt is triggered.
*/

#include "dma.h"

/*!
    \brief      configure DMA peripheral in circular mode, memory increase enable 
    \param      none
    \retval     none
*/
void dma_config(void){

    /** TODO: setup DMA to accuratly get sample x amount of periods */

    // enable DMA clock
    rcu_periph_clock_enable(RCU_DMA0);

    // configure the ADC_DMA channel
    dma_parameter_struct dma_data_param;
    dma_deinit(DMA0, DMA_CH0);
    
    // initialize data mode
    dma_data_param.periph_addr  = (uint32_t)(&ADC_RDATA(ADC0));
    dma_data_param.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_param.memory_addr  = (uint32_t)(&adc_value);
    dma_data_param.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_param.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_data_param.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_data_param.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_data_param.number       = 5;        // NUMBER OF ADC CHANNELS
    dma_data_param.priority     = DMA_PRIORITY_HIGH;
    dma_init(DMA0, DMA_CH0, &dma_data_param);

    // enable circular mode
    dma_circulation_enable(DMA0, DMA_CH0);

    // enable DMA chanel
    dma_channel_enable(DMA0, DMA_CH0);
}
