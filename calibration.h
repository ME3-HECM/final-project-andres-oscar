#ifndef _calibration_H
#define	_calibration_H

#include <xc.h>
#include "color.h"

#define _XTAL_FREQ 64000000


#endif

void calibration_colors(colors *cCal);
unsigned int calibration_turning(struct DC_motor *mL, struct DC_motor *mR);
void customDelayMs(unsigned int milliseconds);

