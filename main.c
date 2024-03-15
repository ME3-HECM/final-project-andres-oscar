/************************************
 * #pragma directives...
************************************/
#pragma config FEXTOSC = HS
#pragma config RSTOSC = EXTOSC_4PLL 
#pragma config WDTE = OFF        

/************************************
 * #include directives...
 ************************************/
#include <xc.h>
#include "color.h"
#include "i2c.h"
#include "ADC.h"
#include "serial.h"
#include "dc_motor.h"
#include "timers.h"
#include "return_func.h"
#include "calibration.h"
#include "maze_navigation.h"

/************************************
 * #define directives...
 ************************************/
#define _XTAL_FREQ 64000000 

/************************************
/ main function
 * ...
************************************/
void main(void) {   
    //Initializing variables
    unsigned int clear_norm;
    unsigned int path_step = 0;
    unsigned int hue;
    unsigned int factorR;
    unsigned int factorL;
    
    //Initialization Sequence
   
    ADC_init();
    color_click_init();
    initUSART4();
    Timer0_init();  
    //initalize buttons used for calibration
    buttons_init();
    //Colored LED initialization
    color_clicker_lights_init();
    //initializing DC motors
    initDCmotorsPWM(99); //insert PWM 
    variablesMotorInit(&motorL, &motorR, 99);

    calibration_colors(&colorCalibration);
    factorR = calibration_turningR(&motorL, &motorR) ;
    factorL = calibration_turningL(&motorL, &motorR) ;


    while (1) {
        //starts moving the car
        intial_stage_movement(&motorL, &motorR);
        
        //looks for clear value to increase in order to stop car, and read values for time taken for the stepHome
        looking_for_card(&motorL,&motorR,&colorCurrent, &colorCalibration, path_step);
        
        path_step++;
        
        __delay_ms(200);

        bump_wall(&motorL, &motorR);

        hue = reading_hue(&colorCurrent);
        
        send2USART(hue);
        
        //second reading after bump of wall
        clear_norm = calc_clear_norm(&colorCurrent, &colorCalibration);
        
        is_white(&motorL, &motorR,path_step, factorR, factorL, hue, clear_norm);
      
        path_step = decision(hue, path_step, factorR, factorL);
    }

            
}

