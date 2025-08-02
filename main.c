
#include "gd32vf103.h"
#include "gd32vf103_it.h"
#include "adc.h"
#include "timer.h"
#include "spi.h"

int main(void){
    // initialize ADC with DMA 
    adc_dma_init();

    // initialize TIMER2 trigger for ADC
    timer2_TRGO_init();

    // initilize SPI as slave 
    spi1_slave_init();    

    // enable eclic to handle interrupt sources 
    eclic_config();

    while(1){
        // interuppts do the rest...
    }
}
