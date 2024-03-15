#ifndef _timers_H
#define _timers_H

#include <xc.h>
#include "timers.h"


#define _XTAL_FREQ 64000000


/********************************************//**
 *  Functions to initialize timer and collect a 16 bit value 
 ***********************************************/
void Timer0_init(void);
void get16bitTMR0val(unsigned int path_step);

#endif
