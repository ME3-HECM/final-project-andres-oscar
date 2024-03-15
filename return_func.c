#include <xc.h>
#include "return_func.h"
#include "dc_motor.h"
#define MAX_PATH_LENGTH 50
/************************************
 * #RETURN FUNCTIONS
 * Whenever a turn is made or the buggy is moving forward, it logs this into a structure that includes the action and the time duration
 * The time duration is important for how long the car needs to reverse in straight sections
 * 
************************************/

//function for logging actions into the path structure
void logAction(char newAction, int newTime, unsigned int path_step) {
    if (path_step < MAX_PATH_LENGTH) { //prevents array overflow
        action[path_step] = newAction; //logs the action
        time[path_step] = newTime; //stores the time of each movement, though only used in straight moves
        
    }

}

//function that reverses the turns in the path for more simplicity in the returnHome function later
void reverseTurn(struct DC_motor *mL, struct DC_motor *mR, char actionStep) {
    if (actionStep == 1) {
        backHalf(mL,mR); //moving back half a unit
        __delay_ms(500); //delay to slow down potential skidding
        left90(mL,mR); //turning left 90 function
    } else if (actionStep == 2) {
        //Move back half a unit and turn right 90
        backHalf(mL,mR); //moving back half a unit
        __delay_ms(500); //delay to slow down potential skidding
        right90(mL,mR); //turning right 90 function
    } else if (actionStep == 3) {
        //Move back half a unit and turns 180
         backHalf(mL,mR); //moving back half a unit
         __delay_ms(500); //delay to slow down potential skidding
        turn180(mL,mR); //turning 180 function
    } else if (actionStep == 4) {
        //Move back half a unit and turn left 135
        backHalf(mL,mR); //moving back half a unit
        __delay_ms(500); //delay to slow down potential skidding
        left135(mL,mR); //turning left 135 function
    } else if (actionStep == 5) {
        backHalf(mL,mR); //moving back half a unit
        __delay_ms(500); //delay to slow down potential skidding
        right135(mL,mR); //turning right 135 function
    }
        
}

//custom delay function for the custom delays when returning
void customDelayMs(unsigned int milliseconds) {
    for (unsigned int i = 0; i < milliseconds; i++) {
        __delay_ms(1); // Delay of 1 ms
    }
}

//function that reverses the straight direction, taking the time in ms required as an input
void reverseStraight(struct DC_motor *mL, struct DC_motor *mR, long time_ms) 
{
    //NEED TO UPDATE TIME SINCE IT WILL BE IN BITS
    unsigned int delayMs = 1000;
    
    //completely arbitrary delays and bit values, needs more testing!
    if (time_ms>5500){delayMs = 3000; } //movement of 3 squares
    else if (time_ms>3000 && time_ms>4100){delayMs = 2000; } //movements of 2 squares
    else if (time_ms<3000){delayMs = 1000;} //movement of 1 square
    
    fullSpeedAhead(&motorL,&motorR);
    customDelayMs(delayMs); //custom delay in ms
    stop(&motorL,&motorR);
    __delay_ms(300); //delay to think
}

//return home function
void returnHome(struct DC_motor *mL, struct DC_motor *mR, unsigned int path_step) 
{
    //turn off white light during normal operation
    LATGbits.LATG0 = 0; //Red is G0
    LATEbits.LATE7 = 0; //Green is E7
    LATAbits.LATA3 = 0; //Blue is A3
        
    moveWhite(&motorL,&motorR);
    
    if (path_step > 0) { // Check to ensure there's at least one action to reverse.
        for (unsigned int i = path_step - 1; ; i--) {
            // Correct time calculation method here if needed.
            long time_ms = time[i]*65535*4*8192/64000000; // Assuming this is already in milliseconds or calculate correctly.
            char action_turn = action[i];

            if (action_turn == 0) { //0 denotes straight movement.
                reverseStraight(mL, mR, time_ms);
            } else {
                reverseTurn(mL, mR, action_turn);
            }

            if (i == 0) break; // Break at 0 to avoid underflow.
        }
    }   
    Sleep();
}
    

