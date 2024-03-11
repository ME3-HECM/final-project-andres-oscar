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
#include "functions.h"

/************************************
 * #define directives...
 ************************************/
#define _XTAL_FREQ 64000000 

/************************************
/ main function
 * ...
************************************/
void main(void) {   
    //Initialization Sequence
    ADC_init();
    color_click_init();
    initUSART4();
    
    //initializing DC motors
    unsigned int PWMcycle = 99;
    initDCmotorsPWM(PWMcycle);

    //set initial values for the motorL and motorR structures
    motorL.power = 0;
    motorL.direction = 1;  
    motorL.brakemode = 1; 
    motorL.PWMperiod = PWMcycle;
    motorL.posDutyHighByte = (unsigned char *)(&CCPR1H); //assign the correct CCP register address
    motorL.negDutyHighByte = (unsigned char *)(&CCPR2H); //assign the correct CCP register address

    motorR.power = 0;
    motorR.direction = 1;  
    motorR.brakemode = 1;
    motorR.PWMperiod = PWMcycle;
    motorR.posDutyHighByte = (unsigned char *)(&CCPR3H); //assign the correct CCP register address
    motorR.negDutyHighByte = (unsigned char *)(&CCPR4H); //assign the correct CCP register address
    
    // setup LEFT pin for output of battery status
    LATDbits.LATD7=0;   //set initial output state
    TRISDbits.TRISD7=0; //set TRIS value for pin (output)
    
    
    //Colored LED initialization
    TRISGbits.TRISG0 = 0;
    LATGbits.LATG0 = 0; //Red is G0
    TRISEbits.TRISE7 = 0;
    LATEbits.LATE7 = 0; //Green is E7
    TRISAbits.TRISA3 = 0;
    LATAbits.LATA3 = 0; //Blue is A3
    
    //finding current battery value, max 255 (we think), so putting it in infinite while loop with LED indicating low battery

    //Buttons on Clicker Board Initializations
    TRISFbits.TRISF3 = 1; // set clicker as input for calibration
    ANSELFbits.ANSELF3 = 0; //turn off analogue input on pin 
    LATFbits.LATF3 = 1;
    
    unsigned int battery_level;
    unsigned int red;
    unsigned int blue;
    unsigned int green;
    unsigned int clear;
    
    //variable to store previous button state
    int prevButtonState = PORTFbits.RF3; //initialize prevButtonState with the current state of the button

    //code stricture for testing the movement functions
    while (1) {
        //read the current button state
        int buttonState = PORTFbits.RF3;
        
        //check if the button state has changed from not pressed to pressed
        if (buttonState == 1 && prevButtonState == 0) {
            __delay_ms(500); //delay to move away from buggy
            //perform the action to be calibrated
            moveOrange(&motorL, &motorR);
        }
        
        //update the previous button state
        prevButtonState = buttonState;

        
    }
}