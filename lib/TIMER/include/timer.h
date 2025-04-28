#pragma once

/*!
    \file   timer.h
    \brief  TIMER2 driver interface   

    This file defines the interface for the timer configuration.
    The timer triggers an TRGO interrupt every millisecond 
*/

#include "gd32vf103.h"

/** Initialize TIMER2 with TRGO interrupt */
void timer2_TRGO_init(void);
