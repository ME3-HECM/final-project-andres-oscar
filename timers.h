#ifndef _timers_H
#define _timers_H

#include <xc.h>
#include "dc_motor.h"


#define _XTAL_FREQ 64000000

void Timer0_init(void);
unsigned int get16bitTMR0val(unsigned int path_step);

#endif
