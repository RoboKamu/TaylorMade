/*!
    \file   adc.c
    \brief  ADC driver implementation

    The idea is to have ADC in scan mode for 5 channels.
    These 5 channels are sampled once triggered by TIMER2 and stored in DMA. 
*/

#include "adc.h"

static void rcu_config(void);
static void gpio_config(void);
static void adc_config(void);

/*!
    \brief      Initialize ADC functionality and RCU with scan mode, continuous disable
    \param      none
    \retval     none
*/
void adc_dma_init(void){
    rcu_config();
    gpio_config();
    dma_ftf_init();
    adc_config();
}

/*!
    \brief      configure relevant peripheral clocks
    \param      none
    \retval     none
*/
static void rcu_config(void){
    // enable GPIOA clock
    rcu_periph_clock_enable(RCU_GPIOA);
    // enable ADC0 clock
    rcu_periph_clock_enable(RCU_ADC0);
    // config ADC clock
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV8);
}

/*!
    \brief      configure GPIOA peripheral for pins A1-A5
    \param      none
    \retval     none
*/
static void gpio_config(void){
    /** For some unclear reason the GPIO_MODE_IPD (pull down input mode) does not work for pin A0.
     * Therefor pins A1-A5 is used instead, and works as expected with relativly stable values. */
    gpio_init(GPIOA, GPIO_MODE_IPD, GPIO_OSPEED_2MHZ, ADC_GPIO_PINS);   
}

/*!
    \brief      configure ADC peripheral in scan operation mode, continuous disable
    \param      none
    \retval     none
*/
static void adc_config(void){

    /** TODO: features
     *  sooner: DONE!
     *      ----Have a conversion at specific timings with a timer to trigger conversion. ----
     *      note: ADC TRGO interrupt only works for TIMER2 not TIMER1 which was the first implementation
     * 
     *  later:
     *      Have 1 channel on ADC0 and 4 channels on ADC1 then synch 
     *      instead of ADC_MODE_FREE so channel 1 is parallell with every ADC1 channel
     *      Then Voltage (A1) would be read in parallell with current (A2-A5) 
     */

    // reset ADC 
    adc_deinit(ADC0);
    // configure ADC independent mode
    adc_mode_config(ADC_MODE_FREE);
    // ADC disable continuous for TIMER based solution 
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, DISABLE);
    // enable ADC scan mode
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
    // ADC data alignment config to MSB
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    // channel length config
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, NUM_OF_ADC_CHANNELS);
    
    // ADC regular channel config
    adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_1, ADC_SAMPLETIME_239POINT5);
    adc_regular_channel_config(ADC0, 1, ADC_CHANNEL_2, ADC_SAMPLETIME_239POINT5);
    adc_regular_channel_config(ADC0, 2, ADC_CHANNEL_3, ADC_SAMPLETIME_239POINT5);
    adc_regular_channel_config(ADC0, 3, ADC_CHANNEL_4, ADC_SAMPLETIME_239POINT5);
    adc_regular_channel_config(ADC0, 4, ADC_CHANNEL_5, ADC_SAMPLETIME_239POINT5);
    
    // ADC timer trigger config, listen for TRGO on TIMER2 
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_EXTTRIG_REGULAR_T2_TRGO);
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);

    // enable ADC interface
    adc_enable(ADC0);
    usb_delay_1ms(1);

    // ADC calibration should be performed before conversion
    adc_calibration_enable(ADC0);

    // enable ADC DMA function
    adc_dma_mode_enable(ADC0);
}
