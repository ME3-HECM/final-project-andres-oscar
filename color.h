#ifndef _color_H
#define _color_H

#include <xc.h>
#include "dc_motor.h"


#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

typedef struct colors { //definition of DC_motor structure
    unsigned int red;   //red colour
    unsigned int green;   //green colour
    unsigned int blue;   //blue colour
    unsigned int clear;   //clear colour
} colors;



struct colors colorCalibration, colorCurrent; 		//declare two color structures 

/********************************************//**
 *  Function to initialise the colour click module using I2C
 ***********************************************/
void color_click_init(void);

/********************************************//**
 *  Function to write to the colour click module
 *  address is the register within the colour click to write to
 *	value is the value that will be written to that address
 ***********************************************/
void color_writetoaddr(char address, char value);

/********************************************//**
 *  Function to read the red channel
 *	Returns a 16 bit ADC value representing colour intensity
 ***********************************************/
unsigned int color_read_Red(void);

/********************************************//**
 *  Function to read the Green channel
 *	Returns a 16 bit ADC value representing colour intensity
 ***********************************************/
unsigned int color_read_Green(void);

/********************************************//**
 *  Function to read the Blue channel
 *	Returns a 16 bit ADC value representing colour intensity
 ***********************************************/
unsigned int color_read_Blue(void);


/********************************************//**
 *  Function to read the Clear channel
 *	Returns a 16 bit ADC value representing colour intensity
 ***********************************************/
unsigned int color_read_Clear(void);

void color_clicker_lights_init(void);

unsigned int reading_hue(colors *cCurr);

unsigned int convert_rgb2hue(colors *cMax, colors *cCurr);

void calibration_routine(colors *cCal);

unsigned int calc_clear_norm(struct colors *cCurr, struct colors *cMax);

unsigned int decision(unsigned int hue, unsigned int path_step, unsigned int factorR, unsigned int factorL);

unsigned int is_white(struct DC_motor *mL, struct DC_motor *mR, unsigned int path_step, unsigned int factorR, unsigned int factorL, unsigned int hue, unsigned int clear_norm);
#endif
