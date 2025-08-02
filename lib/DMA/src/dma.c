/*!
    \file   dma.c
    \brief  DMA driver implementation

    The goal is to have DMA like a circular buffer, continously storing ADC data.
    Once a defined amount of conversions are completed, an interrupt is triggered.

    In this case the signal period is 20ms with 5 sample per ms, for 5 channels the buffer size is 500. 
    Once 500 memeory locations stored, the DMA triggers an full transfer finish interrupt.
*/

#include "dma.h"

static void rcu_config(void);
static void dma_config(void);
static void gpio_config(void);

/*!
    \brief      Initialize the DMA with full transfer interrupt
    \param      none
    \retval     none 
*/
void dma_ftf_init(){
    rcu_config();
    dma_config();
    gpio_config();
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
    // enable GPIO clock
    rcu_periph_clock_enable(RCU_GPIOB);
}

static void gpio_config(void){
    // initialize DMA ready GPIO pin 
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
}
