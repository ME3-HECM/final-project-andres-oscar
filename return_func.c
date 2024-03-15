#include <xc.h>
#include "return_func.h"
#include "dc_motor.h"
#include "color.h"
#define MAX_PATH_LENGTH 50
/************************************
 * #RETURN FUNCTIONS
 * Whenever a turn is made or the buggy is moving forward, it logs this into a structure that includes the action and the time duration
 * The time duration is important for how long the car needs to reverse in straight sections
 * 
************************************/

//function for logging actions into the path structure
void logAction(char newAction, long newTime, unsigned int path_step) {
    if (path_step < MAX_PATH_LENGTH) { //prevents array overflow
        action[path_step] = newAction; //logs the action
        time[path_step] = newTime; //stores the time of each movement, though only used in straight moves
        
    }

}

//function that reverses the turns in the path for more simplicity in the returnHome function later
void reverseTurn(struct DC_motor *mL, struct DC_motor *mR, char actionStep, long time_ms, unsigned int factor) {
    if (actionStep == 1) { //red
        //Turns left 90 and goes back half a unit
        
        left90(mL,mR, factor); //turning left 90 function
        __delay_ms(500); //delay to slow down potential skidding
        backHalf(mL,mR); //moving back half a unit
        
    } else if (actionStep == 2) { //blue
        //Turns right 90 and goes back half a unit
        
        right90(mL,mR, factor); //turning right 90 function
        __delay_ms(500); //delay to slow down potential skidding
        backHalf(mL,mR); //moving back half a unit
        
    } else if (actionStep == 3) { //green
        //Turns 180 and goes back half a unit
         
        turn180(mL,mR, factor); //turning 180 function
         __delay_ms(500); //delay to slow down potential skidding
        backHalf(mL,mR); //moving back half a unit
        
    } else if (actionStep == 4) { //ligth blue
        //Turns left 135 and goes back half a unit
        
        right135(mL,mR, factor); //turning left 135 function
        __delay_ms(500); //delay to slow down potential skidding
        backHalf(mL,mR); //moving back half a unit
        
    } else if (actionStep == 5) { //yellow
        //Turns right 135 and goes back half a unit
        
        left90(mL,mR, factor); //turning right 135 function
        __delay_ms(500); //delay to slow down potential skidding
        backHalf(mL,mR); //moving back half a unit
        
    } else if (actionStep == 6) { //orange
        //Turns right 135 and goes back half a unit
        
        left135(mL,mR, factor); //turning right 135 function
        __delay_ms(500); //delay to slow down potential skidding
        backHalf(mL,mR); //moving back half a unit
        
    } else if (actionStep == 7) {//pink
        //Turns right 135 and goes back half a unit
        
        right90(mL,mR, factor); //turning right 135 function
        __delay_ms(500); //delay to slow down potential skidding
        backHalf(mL,mR); //moving back half a unit
        
}
}



//function that reverses the straight direction, taking the time in ms required as an input
void reverseStraight(struct DC_motor *mL, struct DC_motor *mR, long time_ms) 
{
    //NEED TO UPDATE TIME SINCE IT WILL BE IN BITS
    unsigned int delayMs = 1000;
    
    //completely arbitrary delays and bit values, needs more testing!
    if (time_ms>6500){delayMs = 7750; } //movement of 3 squares
    else if (time_ms>4000 && time_ms<6500){delayMs = 5500; } //movements of 2 squares
    else if (time_ms<4000){delayMs = 2750;} //movement of 1 square
    
    fullSpeedAhead(&motorL,&motorR);
    customDelayMs(delayMs); //custom delay in ms
    stop(&motorL,&motorR);
    __delay_ms(300); //delay to think
}

//return home function
void returnHome(struct DC_motor *mL, struct DC_motor *mR, unsigned int path_step, unsigned int factor) 
{
    
    long time_ms;

    //turn off white light during normal operation
    LATGbits.LATG0 = 0; //Red is G0
    LATEbits.LATE7 = 0; //Green is E7
    LATAbits.LATA3 = 0; //Blue is A3
        
    moveWhite(&motorL,&motorR,factor);
    
    if (path_step > 0) { // Check to ensure there's at least one action to reverse.
        for (unsigned int i = path_step - 1; ; i--) {
            // Correct time calculation method here if needed.
            char action_turn = action[i];
            if (action[i+1]== 5 || action[i+1]==7){
                time_ms = time[i]-2250; //calibrate this to how much a decrease is
            }else{
                time_ms = time[i];
        }
            if (action_turn == 0) { //0 denotes straight movement.
                reverseStraight(mL, mR, time_ms);
            } else {
                reverseTurn(mL, mR, action_turn, time_ms, factor);
            }

            if (i == 0) break; // Break at 0 to avoid underflow.
        }
    }   
    Sleep();
}
    

