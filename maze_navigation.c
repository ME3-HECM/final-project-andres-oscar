#include <xc.h>
#include "dc_motor.h"
#include "color.h"
#include "timers.h"
#include <stdio.h>
#include "maze_navigation.h"



/************************************
 * #COLOR MOVEMENT FUNCTIONS! for tuning look above at set pieces
 * This is where you log actions into the path
************************************/

void  moveRed(struct DC_motor *mL, struct DC_motor *mR, unsigned int factorR)
{
    //Move back half a unit and turn right 90
    backHalf(mL,mR); //moving back half a unit
    __delay_ms(500); //delay to slow down potential skidding
    right90(mL,mR,factorR); //turning right 90 function


    
}

void moveGreen(struct DC_motor *mL, struct DC_motor *mR, unsigned int factorL)
{
    //Move back half a unit and turn left 90
    backHalf(mL,mR); //moving back half a unit
    __delay_ms(500); //delay to slow down potential skidding
    left90(mL,mR,factorL); //turning left 90 function

}

void moveBlue(struct DC_motor *mL, struct DC_motor *mR)
{
    //Move back half a unit and turns 180
    backHalf(mL,mR); //moving back half a unit
    __delay_ms(500); //delay to slow down potential skidding
    turn180(mL,mR); //turning 180 function

}

 void moveYellow(struct DC_motor *mL, struct DC_motor *mR, unsigned int factorR)
{
    //Move back one and a half units and turn right 90
    backOneAndHalf(mL,mR); //moving back half a unit
    __delay_ms(500); //delay to slow down potential skidding
    right90(mL,mR, factorR); //turning right 90 function
}


void  movePink(struct DC_motor *mL, struct DC_motor *mR, unsigned int factorL)
{
    //Move back one and a half units and turn left 90
    backOneAndHalf(mL,mR); //moving back half a unit
    __delay_ms(500); //delay to slow down potential skidding
    left90(mL,mR, factorL); //turning left 90 function

}

void moveOrange(struct DC_motor *mL, struct DC_motor *mR)
{
    //Move back half a unit and turn right 135
    backHalf(mL,mR); //moving back half a unit
    __delay_ms(500); //delay to slow down potential skidding
    right135(mL,mR); //turning right 135 function

}

void moveLightBlue(struct DC_motor *mL, struct DC_motor *mR)
{
    //Move back half a unit and turn left 135
    backHalf(mL,mR); //moving back half a unit
    __delay_ms(500); //delay to slow down potential skidding
    left135(mL,mR); //turning left 135 function


}

void moveWhite(struct DC_motor *mL, struct DC_motor *mR)
{
    //Move back half a unit and turn around
    backHalf(mL,mR); //moving back half a unit
    __delay_ms(500); //delay to slow down potential skidding
    turn180(mL,mR); //turning around 180
    backHalf(mL,mR); //moving back half a unit

}

void intial_stage_movement(struct DC_motor *mL, struct DC_motor *mR)
{
        //turn on white light during normal operation
        LATGbits.LATG0 = 1; //Red is G0
        LATEbits.LATE7 = 1; //Green is E7
        LATAbits.LATA3 = 1; //Blue is A3
        
        //moves forward
        fullSpeedAhead(mL,mR);
        T0CON0bits.T0EN=1;	//start the timer
}

void looking_for_card(struct DC_motor *mL, struct DC_motor *mR, struct colors *cCurr, struct colors *cMax, unsigned int path_step)
{
    unsigned int clear_norm=0;
    while(clear_norm<10){ //change the addition depending on ambient
        clear_norm = calc_clear_norm(cCurr, cMax);
    }

    T0CON0bits.T0EN=0;	//stops the timer

    stop(mL,mR); //stops moving

    get16bitTMR0val(path_step); //takes the timer value at that instant
  
}

void bump_wall(struct DC_motor *mL, struct DC_motor *mR)
{
    //small sequence to bump wall for better readings
    fullSpeedAhead(mL,mR);
    __delay_ms(300);
    stop(mL,mR);
    __delay_ms(300);
}
