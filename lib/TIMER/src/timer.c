/*!
    \file   timer.c
    \brief  TIMER2 driver implementation

    50 Hz input signal, 1 sample / ms on 5 channels, with adc in scan mode.
    The timers role is to trigger the ADC to start a conversion on the 5 channels.
    For 1 sample / ms a interrupt is triggered every ms with TIMER2 TRGO
*/

#include "timer.h"

static void rcu_config(void);
static void timer_config(void);

/*!
    \brief      Initialize TIMER2 with TRGO interrupt
    \param      none
    \retval     none
*/
void timer2_TRGO_init(void){
    rcu_config();
    timer_config();
}

/*!
    \brief      configure peripheral clock
    \param      none
    \retval     none
*/
static void rcu_config(void){
    rcu_periph_clock_enable(RCU_TIMER2);
}


/*!
    \brief      configure TIMER2 peripheral
    \param      none
    \retval     none
*/
static void timer_config(void){
    /** For the timer parameters the timer_parameter_struct is configured. 
     *  A prescaler of 95 is set for the 96 MHz systemclock, results in 1 MHz PSC_CLK.
     *  Goal is start from 0 and count to period then counter overflow. This works with
     *  counter up counting with edge alignment - meaning only counting up to the overflow, and 
     *  with no repetitioncounter set. 
     *  The timer should not have any dead time so clockdivision is set to TIMER_CKDIV_DIV1.*/

    timer_parameter_struct timer_initpara;

    // deinit current TIMER2 
    timer_deinit(TIMER2);
    // inintialize TIMER2 with parameters struct 
    timer_struct_para_init(&timer_initpara);
    // TIMER2 parameter struct configuation
    timer_initpara.prescaler            = 95;   // each step 1us 
    timer_initpara.alignedmode          = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection     = TIMER_COUNTER_UP;
    timer_initpara.period               = 999;  // overflow after 1000us (autoreload every ms)
    timer_initpara.clockdivision        = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter    = 0;
    timer_init(TIMER2, &timer_initpara);

    // Setup TIMER2 to generate hardware event (TRGO) to trigger ADC
    timer_master_output_trigger_source_select(TIMER2, TIMER_TRI_OUT_SRC_UPDATE);
    
    timer_enable(TIMER2);
}
