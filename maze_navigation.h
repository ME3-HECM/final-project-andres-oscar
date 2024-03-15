#ifndef _MAZE_NAVIGATION_H
#define _MAZE_NAVIGATION_H

#include <xc.h>
#include "maze_navigation.h"

#define _XTAL_FREQ 64000000
//color movements
void moveRed(struct DC_motor *mL, struct DC_motor *mR, unsigned int factorR);
void moveGreen(struct DC_motor *mL, struct DC_motor *mR, unsigned int factorL);
void moveBlue(struct DC_motor *mL, struct DC_motor *mR);
void moveYellow(struct DC_motor *mL, struct DC_motor *mR, unsigned int factorR);
void movePink(struct DC_motor *mL, struct DC_motor *mR, unsigned int factorL);
void moveOrange(struct DC_motor *mL, struct DC_motor *mR);
void moveLightBlue(struct DC_motor *mL, struct DC_motor *mR);
void moveWhite(struct DC_motor *mL, struct DC_motor *mR);

//stages within maze
void intial_stage_movement(struct DC_motor *mL, struct DC_motor *mR);
void bump_wall(struct DC_motor *mL, struct DC_motor *mR);
void looking_for_card(struct DC_motor *mL, struct DC_motor *mR, struct colors *cCurr, struct colors *cMax, unsigned int path_step);




#endif	

