#include "gd32vf103_it.h"

#define NUM_OF_ADC_CHANNELS     5
#define PERIOD_LENGTH           20  // 20ms = 1 period
#define OLD_BUFFER_LENGTH           NUM_OF_ADC_CHANNELS * PERIOD_LENGTH // 100
#define BUFFER_LENGTH           5 * OLD_BUFFER_LENGTH

#define START_BYTE_ONE      0x01
#define START_BYTE_TWO      0xA4
#define START_BYTE_LEN      2

#define SPI_SIZE        BUFFER_LENGTH * 2 + START_BYTE_LEN // uint16 to byte array

/** SPI sending buffer */
uint8_t spi1_send_array[SPI_SIZE];

/** SPI index */
uint16_t spi_n = 0;

extern uint16_t dma_buffer[];

/*!
    \brief      Adds the 2 start bytes to align data on master
    \param      spi_arr:    array to send 
    \param      spi_n:     pointer to location where start flag will be set
    \retval     none
*/
void add_start_flag(uint8_t *spi_arr, uint16_t *spi_n) {
    spi_arr[*spi_n] = START_BYTE_ONE;
    (*spi_n)++;
    spi_arr[*spi_n] = START_BYTE_TWO;
    (*spi_n)++;
}

/*!
    \brief      Convert unsiged 16 bit array to unsigned 8 bit array
    \param      dest:   new flattened array will be stored here 
    \param      src:    source of array to flatten
    \retval     none
    \note       Assumes dest initialized to twice as large array as src
*/
void flatten_array(uint8_t *dest, uint16_t *src, uint16_t start_idx){
    uint16_t n = start_idx;
    for (uint16_t i = 0; i < BUFFER_LENGTH; i++){
        uint8_t hi = src[i] >> 8;
        uint8_t lo = (uint8_t) src[i];
        dest[n++] = hi;
        dest[n++] = lo;
    }   
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

    uint16_t dma_cache[BUFFER_LENGTH];
    
    for (uint16_t i=0; i < BUFFER_LENGTH; i++){
        // channel 1-5 scaling
        uint8_t channel = (i%NUM_OF_ADC_CHANNELS);
        dma_cache[i] = dma_buffer[i] | ( (channel+1) << 12 );  
    }

    spi_n = 0;
    add_start_flag(spi1_send_array, &spi_n);
    flatten_array(spi1_send_array, dma_cache, spi_n);
    spi_n = 0;

    // notify the master that spi buffer is ready for transmission
    gpio_bit_write(GPIOB, GPIO_PIN_0, !gpio_output_bit_get(GPIOB, GPIO_PIN_0));
}

/*!
    \brief      ISR for SPI data transmission
    \param      none
    \retval     none 
*/
void SPI1_IRQHandler(void){
    if (RESET != spi_i2s_interrupt_flag_get(SPI1, SPI_I2S_INT_FLAG_TBE)){
        // TBE interupt is active, verify buffer empty via hardware status bit
        while (RESET == spi_i2s_flag_get(SPI1, SPI_FLAG_TBE));
        
        spi_i2s_data_transmit(SPI1, spi1_send_array[spi_n++]);
        //if (spi_n >= TEST_ARRAYSIZE) spi_n = 0;
        if (spi_n >= SPI_SIZE) spi_n = 0;
    }
}

/*!
    \brief      configure ECLIC interrupt handling scheme for peripherals
    \param      none
    \retval      none
*/
void eclic_config(void){
    eclic_global_interrupt_enable();
    eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO1);
    eclic_irq_enable(SPI1_IRQn, 1, 0);
    eclic_irq_enable(DMA0_Channel0_IRQn, 1, 1);
}
