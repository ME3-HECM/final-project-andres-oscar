#include <xc.h>
#include "return_func.h"
#include "dc_motor.h"
#include "color.h"
#include "maze_navigation.h"
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
void reverseTurn(struct DC_motor *mL, struct DC_motor *mR, char actionStep, long time_ms, unsigned int factorR, unsigned int factorL) {
    if (actionStep == 1) { //red
        //Turns left 90 and goes back half a unit
      
        left90(mL,mR, factorL); //turning left 90 function
        __delay_ms(500); //delay to slow down potential skidding
        backHalf(mL,mR); //moving back half a unit
        backHalf(mL,mR); //moving back half a unit

        
    } else if (actionStep == 2) { //blue
        //Turns right 90 and goes back half a unit
        
        right90(mL,mR, factorR); //turning right 90 function
        __delay_ms(500); //delay to slow down potential skidding
        backHalf(mL,mR); //moving back half a unit
        backHalf(mL,mR); //moving back half a unit

        
    } else if (actionStep == 3) { //green
        //Turns 180 and goes back half a unit
         
        turn180(mL,mR); //turning 180 function
         __delay_ms(500); //delay to slow down potential skidding
        backHalf(mL,mR); //moving back half a unit
        backHalf(mL,mR); //moving back half a unit

        
    } else if (actionStep == 4) { //light blue
        //Turns left 135 and goes back half a unit
        
        right135(mL,mR); //turning left 135 function
        __delay_ms(500); //delay to slow down potential skidding
        backHalf(mL,mR); //moving back half a unit
        backHalf(mL,mR); //moving back half a unit

        
    } else if (actionStep == 5) { //yellow
        //Turns right 135 and goes back half a unit
        
        left90(mL,mR, factorL); //turning right 135 function
        __delay_ms(500); //delay to slow down potential skidding
        
    } else if (actionStep == 6) { //orange
        //Turns right 135 and goes back half a unit
        
        left135(mL,mR); //turning right 135 function
        __delay_ms(500); //delay to slow down potential skidding
        backHalf(mL,mR); //moving back half a unit
        backHalf(mL,mR); //moving back half a unit

        
    } else if (actionStep == 7) {//pink
        //Turns right 135 and goes back half a unit
        
        right90(mL,mR, factorR); //turning right 135 function
        __delay_ms(500); //delay to slow down potential skidding
        
}
}



//function that reverses the straight direction, taking the time in ms required as an input
void reverseStraight(struct DC_motor *mL, struct DC_motor *mR, long time_ms) 
{
    //time is already in ms
    unsigned int delayMs = 1550;
    
    //expressing ranges of the timer value that corresponding to delay by a certain number of cells
    //this makes the return function more robust, assuming maximum movement of 4 squares
    if (time_ms>150000 && time< 15000){delayMs = 6200; } //movement of 4 squares
    else if (time_ms>12000 && time< 15000){delayMs = 44000; } //movement of 3 squares
    else if (time_ms>10000 && time_ms<12000){delayMs = 2750; } //movements of 2 squares
    else if (time_ms<10000){delayMs = 1550;} //movement of 1 square
     
    //goes forward for the designated time period
    fullSpeedAhead(&motorL,&motorR); 
    customDelayMs(delayMs); //custom delay in ms
    stop(&motorL,&motorR);
    __delay_ms(300); //delay to think
}

//return home function, recieves motor structures and the calibrated delays for turns
void returnHome(struct DC_motor *mL, struct DC_motor *mR, unsigned int path_step, unsigned int factorR, unsigned int factorL) 
{
    //setting variables
    long time_ms;

    //does a 360 once it reads white
    moveWhite(&motorL,&motorR);
    
    if (path_step > 0) { //check to ensure there's at least one action to reverse.
        for (unsigned int i = path_step - 1; ; i--) {
            //correct time calculation method here if needed.
            char action_turn = action[i];
            
            //if statement to optimise our return function
            if (action[i+1]== 5 || action[i+1]==7){
                time_ms = time[i]-2250; //calibrated to decrease by a single unit cell
            }else{
                time_ms = time[i];
        }
           
            if (action_turn == 0) { //0 denotes straight movement.
                reverseStraight(mL, mR, time_ms);
            } else { //else its a turn so it goes into the reverse turn function
                reverseTurn(mL, mR, action_turn, time_ms, factorR, factorL);
            }

            if (i == 0) break; //break at 0 to avoid underflow.
        }
    }   
    Sleep(); //sleeps for energy conservation
}
    

