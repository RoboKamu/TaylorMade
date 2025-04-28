
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

            char ch1_buf[100]; size_t offset_ch1=0;
            char ch2_buf[100]; size_t offset_ch2=0;
            char ch3_buf[100]; size_t offset_ch3=0;
            char ch4_buf[100]; size_t offset_ch4=0;
            char ch5_buf[100]; size_t offset_ch5=0;

            for (uint8_t i=0; i<PERIOD_LENGTH; i++){
                offset_ch1 += snprintf(ch1_buf+offset_ch1, 100-offset_ch1, "%d,", adc0_ch1_values[i]);
                offset_ch2 += snprintf(ch2_buf+offset_ch2, 100-offset_ch2, "%d,", adc0_ch2_values[i]);
                offset_ch3 += snprintf(ch3_buf+offset_ch3, 100-offset_ch3, "%d,", adc0_ch3_values[i]);
                offset_ch4 += snprintf(ch4_buf+offset_ch4, 100-offset_ch4, "%d,", adc0_ch4_values[i]);
                offset_ch5 += snprintf(ch5_buf+offset_ch5, 100-offset_ch5, "%d,", adc0_ch5_values[i]);
            }
            
            // output =>  chx: 0,0,1,0.....,0 <-- 20 sample values
            snprintf(buffer, USB_BUFFER_SIZE, "ch1: %s\nch2: %s\nch3: %s\nch4: %s\nch5: %s\n",  
                                                ch1_buf, ch2_buf, ch3_buf, ch4_buf, ch5_buf);

            printf("%s\n", buffer);
            fflush(0);
        }
    }
}
