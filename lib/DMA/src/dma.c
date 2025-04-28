/*!
    \file   dma.c
    \brief  DMA driver implementation

    The goal is to have DMA like a circular buffer, continously storing ADC data.
    Once a defined amount of transers (signal periods) are completed, an interrupt is triggered.

    In this case the signal period is 20ms with 1 sample per ms, for 5 channels the buffer size is 100. 
    Once 100 memeory locations stored, the DMA triggers an full transfer finish interrupt.
    Lastly the handler can then store in respective channel buffer and note the buffer as 'ready'.
*/

#include "dma.h"

uint16_t* adc_channels[5] = {
    adc0_ch1_values,
    adc0_ch2_values,
    adc0_ch3_values,
    adc0_ch4_values,
    adc0_ch5_values
};

static void rcu_config(void);
static void eclic_config(void);
static void dma_config(void);

/*!
    \brief      Initialize the DMA with full transfer interrupt
    \param      none
    \retval     none 
*/
void dma_ftf_init(){
    buffer_ready = 0;
    rcu_config();
    eclic_config();
    dma_config();
}

/*!
    \brief      configure DMA peripheral in circular mode, memory increase enable 
    \param      none
    \retval     none
*/
static void dma_config(void){
    // configure the ADC_DMA channel
    dma_parameter_struct dma_data_param;
    dma_deinit(DMA0, DMA_CH0);
    
    // initialize data mode
    dma_data_param.periph_addr  = (uint32_t)(&ADC_RDATA(ADC0));
    dma_data_param.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_param.memory_addr  = (uint32_t)(&dma_buffer);
    dma_data_param.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_param.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_data_param.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_data_param.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_data_param.number       = BUFFER_LENGTH;
    dma_data_param.priority     = DMA_PRIORITY_HIGH;
    dma_init(DMA0, DMA_CH0, &dma_data_param);

    // enable circular mode
    dma_circulation_enable(DMA0, DMA_CH0);

    // enable DMA interrupt for once the transfer is completed (full transfer)
    dma_interrupt_enable(DMA0, DMA_CH0, DMA_INT_FTF);
     
    // enable DMA chanel
    dma_channel_enable(DMA0, DMA_CH0);
}

/*!
    \brief      configure peripheral clock
    \param      none
    \retval     none
*/
static void rcu_config(void){
    // enable DMA0 clock
    rcu_periph_clock_enable(RCU_DMA0);
}

/*!
    \brief      configure interrupt controller
    \param      none
    \retval     none
*/
static void eclic_config(void){
    eclic_global_interrupt_enable();
    eclic_irq_enable(DMA0_Channel0_IRQn, 1, 1);
}

/*!
    \brief      DMA interrupt handler for reading DMA buffer
    \param      none
    \retval     none
*/
void DMA0_Channel0_IRQHandler(void){
    if (!dma_interrupt_flag_get(DMA0, DMA_CH0, DMA_INT_FTF)) return;
    
    // reset DMA full transfer interrupt 
    dma_flag_clear(DMA0, DMA_CH0, DMA_INT_FTF);
    
    for (uint8_t i=0; i < BUFFER_LENGTH; i++){
        // channel 1-5 scaling
        int channel = (i%NUM_OF_ADC_CHANNELS);
        // index 0-4 scaling
        int sample_idx = i/5;
        
        adc_channels[channel][sample_idx] = dma_buffer[i];
    }

    // mark the buffer as ready for the superloop
    buffer_ready = 1;
}
