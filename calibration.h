#ifndef _calibration_H
#define	_calibration_H

#include <xc.h>
#include "color.h"

#define _XTAL_FREQ 64000000

/********************************************//**
 *  Function to write to the colour click module
 *  address is the register within the colour click to write to
 *	value is the value that will be written to that address
 ***********************************************/
void calibration_colors(colors *cCal);
unsigned int calibration_turningR(struct DC_motor *mL, struct DC_motor *mR);
unsigned int calibration_turningL(struct DC_motor *mL, struct DC_motor *mR);
void buttons_init(void);
void customDelayMs(unsigned int milliseconds);



#endif



