#ifndef _calibration_H
#define	_calibration_H

#include <xc.h>
#include "color.h"

#define _XTAL_FREQ 64000000

/********************************************//**
 *  Functions to calibrate the Buggy
 *  First function flashes red, green, blue, then white light and takes maximum readings
 *  Following 2 functions 
 *  Also a custom delay function to vary delays during buggy operation, and initializing buttons for the main
 ***********************************************/
void calibration_colors(colors *cCal);
unsigned int calibration_turningR(struct DC_motor *mL, struct DC_motor *mR);
unsigned int calibration_turningL(struct DC_motor *mL, struct DC_motor *mR);
void buttons_init(void); 
void customDelayMs(unsigned int milliseconds);



#endif



