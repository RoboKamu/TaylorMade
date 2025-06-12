
#include "gd32vf103.h"
#include "usb_serial_if.h"
#include "usb_delay.h"
#include "adc.h"
#include "timer.h"

#include <stdlib.h>
#include <string.h>

#define USE_USB
#define USE_USB_PRINTF
#define USB_BUFFER_SIZE 512 // from usb_serial_if.c
#define BUFF_LEN        123 // ((sign + val)= 5 chars)) * 20 samples + (chx: = 4 chars) + 19 commas

extern uint16_t adc_value[];

int main(void){

    // initialize ADC with DMA 
    adc_dma_init();
    // initialize TIMER2 
    timer2_TRGO_init();
    // setup virtual com port
    configure_usb_serial();

    // wait for successful Serial connection
    while(!usb_serial_available())usb_delay_1ms(10);

    while(1){
        
        if (buffer_ready){  // every 20ms...
            buffer_ready=0;            
            
            char buffer[USB_BUFFER_SIZE];

            char ch1_buf[BUFF_LEN]; size_t offset_ch1=0;
            char ch2_buf[BUFF_LEN]; size_t offset_ch2=0;
            char ch3_buf[BUFF_LEN]; size_t offset_ch3=0;
            char ch4_buf[BUFF_LEN]; size_t offset_ch4=0;
            char ch5_buf[BUFF_LEN]; size_t offset_ch5=0;

            for (uint8_t i=0; i<PERIOD_LENGTH; i++){
                offset_ch1 += snprintf(ch1_buf+offset_ch1, BUFF_LEN-offset_ch1, "%d,", adc0_ch1_values[i]-2048);
                offset_ch2 += snprintf(ch2_buf+offset_ch2, BUFF_LEN-offset_ch2, "%d,", adc0_ch2_values[i]-2048);
                offset_ch3 += snprintf(ch3_buf+offset_ch3, BUFF_LEN-offset_ch3, "%d,", adc0_ch3_values[i]-2048);
                offset_ch4 += snprintf(ch4_buf+offset_ch4, BUFF_LEN-offset_ch4, "%d,", adc0_ch4_values[i]-2048);
                offset_ch5 += snprintf(ch5_buf+offset_ch5, BUFF_LEN-offset_ch5, "%d,", adc0_ch5_values[i]-2048);
            }
            
            printf("ch1: %s\n", ch1_buf);
            printf("ch2: %s\n", ch2_buf);
            printf("ch3: %s\n", ch3_buf);
            printf("ch4: %s\n", ch4_buf);
            printf("ch5: %s\n", ch5_buf);
            
            fflush(0);
        }
    }
}
