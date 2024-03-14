#ifndef _return_func_H
#define	_return_func_H

#include <xc.h>
#include "dc_motor.h"


#define _XTAL_FREQ 64000000
#define MAX_PATH_LENGTH 50  // variable for the maximum length of buggy path

char action[MAX_PATH_LENGTH];
int time[MAX_PATH_LENGTH];

//returning related
void logAction(char newAction, int newTime, unsigned int path_step);
void reverseTurn(struct DC_motor *mL, struct DC_motor *mR, char actionStep); 
void reverseStraight(struct DC_motor *mL, struct DC_motor *mR, long time_ms);
void returnHome(struct DC_motor *mL, struct DC_motor *mR, unsigned int path_step);
void customDelayMs(unsigned int milliseconds);


#endif

