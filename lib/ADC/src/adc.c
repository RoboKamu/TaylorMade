/*!
    \file   adc.c
    \brief  ADC driver implementation

    The idea is to have ADC in scan mode for 5 channels.
    These 5 channels are samples continously and stored in DMA. 
    
    50 Hz input signal, 1 sample / ms on 5 channels => fs = 5*1 KHz = 5 KHz
    Therefore, a conversion should occur every 200 micro seconds. 
*/

#include "adc.h"
#include "dma.h"

void rcu_config(void);
void gpio_config(void);
void adc_config(void);

/*!
    \brief      Initialize ADC functionality and clocks with scan mode, continous enable
    \param      none
    \retval     none
*/
void adc_dma_init(void){
    rcu_config();
    gpio_config();
    // eclic_config();
    // timer_config();
    dma_config();   
    adc_config();
}

/*!
    \brief      configure relevant system clocks
    \param      none
    \retval     none
*/
void rcu_config(void){
    // enable GPIOA clock
    rcu_periph_clock_enable(RCU_GPIOA);
    // enable ADC0 clock
    rcu_periph_clock_enable(RCU_ADC0);
    
    /*! TODO: trigger with timer instead of external */
    // enable timer1 clock
    // rcu_periph_clock_enable(RCU_TIMER1);
    
    // config ADC clock
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV8);
}

/*!
    \brief      configure GPIOA peripheral for pins A1-A5
    \param      none
    \retval     none
*/
void gpio_config(void){
    /** For some unclear reason the GPIO_MODE_IPD (pull down input mode) does not work for pin A0.
     * Therefor pins A1-A5 is used instead, and works as expected with relativly stable values. */
    gpio_init(GPIOA, GPIO_MODE_IPD, GPIO_OSPEED_2MHZ, ADC_GPIO_PINS);   
}

/*!
    \brief      configure ADC peripheral in scan operation mode, continous enable
    \param      none
    \retval     none
*/
void adc_config(void){

    /** TODO: features
     *  sooner:
     *      Have a conversion at specific timings with a timer to trigger conversion.
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
    // ADC enable continous 
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);
    // enable ADC scan mode
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
    // ADC data alignment config to MSB
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    // channel length config
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, ADC_NUMBER);
    
    // ADC regular channel config
    adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_1, ADC_SAMPLETIME_239POINT5);
    adc_regular_channel_config(ADC0, 1, ADC_CHANNEL_2, ADC_SAMPLETIME_239POINT5);
    adc_regular_channel_config(ADC0, 2, ADC_CHANNEL_3, ADC_SAMPLETIME_239POINT5);
    adc_regular_channel_config(ADC0, 3, ADC_CHANNEL_4, ADC_SAMPLETIME_239POINT5);
    adc_regular_channel_config(ADC0, 4, ADC_CHANNEL_5, ADC_SAMPLETIME_239POINT5);
    
    // ADC trigger config ( replace with timer later)
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_EXTTRIG_REGULAR_NONE);
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);

    /* 16 times sample, 4 bits shift */
    // adc_oversample_mode_config(ADC0, ADC_OVERSAMPLING_ALL_CONVERT, ADC_OVERSAMPLING_SHIFT_4B, ADC_OVERSAMPLING_RATIO_MUL16);
    // adc_oversample_mode_enable(ADC0);

    // enable ADC interface
    adc_enable(ADC0);
    usb_delay_1ms(1);

    // ADC calibration should be performed before conversion
    adc_calibration_enable(ADC0);

    // enable ADC DMA function
    adc_dma_mode_enable(ADC0);
}
