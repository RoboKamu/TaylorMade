
#include "gd32vf103.h"
#include "usb_serial_if.h"
#include "usb_delay.h"
#include "adc.h"

#include <stdlib.h>
#include <string.h>

#define USE_USB
#define USE_USB_PRINTF

extern uint16_t adc_value[];

int main(void){

    // initialize adc 
    adc_dma_init();
    
    /* This sets up so that the MCU will show up as a virtual com port when connected over usb to a computer */
    configure_usb_serial();

    /* If the controller has sucessfully been connected usb_serial_available() will return 1 */
	while(!usb_serial_available())usb_delay_1ms(10);

    while(1){
        usb_delay_1ms(100);
        
        adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
    
        printf("ADC0 regular channel scan mode, continous enable: \n");
        
        printf("ADC0 regular channel 1 data = %d \n", adc_value[0]);
        printf("ADC0 regular channel 2 data = %d \n", adc_value[1]);
        printf("ADC0 regular channel 3 data = %d \n", adc_value[2]);
        printf("ADC0 regular channel 4 data = %d \n", adc_value[3]);
        printf("ADC0 regular channel 5 data = %d \n", adc_value[4]);
        fflush(0);
    }
}
