/*!
    \file  gd32vf103_it.c
    \brief main interrupt service routines

    \version 2019-6-5, V1.0.0, demo for GD32VF103
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32vf103.h"
#include "../include/drv_usbd_int.h"
#include "../include/drv_usb_hw.h"
#include "../include/gd32vf103_usb_it.h"
#include "../include/usb_serial_if.h"

#define DEBUG_SP() asm volatile("csrrw sp, mscratch, sp")
//#define DEBUG_PIN_ON() gpio_bit_set(GPIOB, GPIO_PIN_0)
//#define DEBUG_PIN_OFF() gpio_bit_reset(GPIOB, GPIO_PIN_0)

extern usb_core_driver USB_OTG_dev;
extern uint32_t usbfs_prescaler;

extern void usb_timer_irq(void);

/*!
    \brief      this function handles USBD interrupt//
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBFS_IRQHandler (void)
{ 
    //DEBUG_PIN_ON();
    usbd_isr (&USB_OTG_dev);
}



/*!
    \brief      this function handles USBD wakeup interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBFS_WKUP_IRQHandler(void)
{
    //DEBUG_PIN_ON();
    if (USB_OTG_dev.bp.low_power) {
        //SystemInit();
        set_usb_clock_96m_hxtal();
        rcu_usb_clock_config(usbfs_prescaler);

        rcu_periph_clock_enable(RCU_USBFS);

        usb_clock_active(&USB_OTG_dev);
    }

    exti_interrupt_flag_clear(EXTI_18);
}


