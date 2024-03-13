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
#include "timers.h"
#include "interrupts.h"

/************************************
 * #define directives...
 ************************************/
#define _XTAL_FREQ 64000000 

/************************************
/ main function
 * ... HERE WRITE WHAT THE MAIN ACHIEVES
************************************/
void main(void) {   
    //Initialization Sequence
    ADC_init();
    color_click_init();
    initUSART4();
    Timer0init();
    Interrupts_init();
    
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
    
    //turning on the front lights
//    TRISDbits.TRISD3 = 0; 
//    LATDbits.LATD3 = 1; 
    
    //Colored LED initialization
    TRISGbits.TRISG0 = 0;
    LATGbits.LATG0 = 0; //Red is G0
    TRISEbits.TRISE7 = 0;
    LATEbits.LATE7 = 0; //Green is E7
    TRISAbits.TRISA3 = 0;
    LATAbits.LATA3 = 0; //Blue is A3
    
    //finding current battery value, max 255 (we think), so putting it in infinite while loop with LED indicating low battery
    
    //Buttons on Clicker Board Initializations
    TRISFbits.TRISF3=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF3=0; //turn off analogue input on pin
    
    unsigned int battery_level;
    unsigned int red;
    unsigned int blue;
    unsigned int green;
    unsigned int clear;
    unsigned int hue;
    char hue_char[20];
    TRISHbits.TRISH3 = 0;
    LATHbits.LATH3 = 1;
    __delay_ms(300);
    LATHbits.LATH3 = 0;
    LATFbits.LATF2 = 0;
    TRISFbits.TRISF2 = 1;
    ANSELFbits.ANSELF2 = 0;
    
    
    calibration_routine(&colorCalibration);
    
    
    while (1) {
        //turn on white light during normal operation
        LATGbits.LATG0 = 1; //Red is G0
        LATEbits.LATE7 = 1; //Green is E7
        LATAbits.LATA3 = 1; //Blue is A3
        fullSpeedAhead(&motorL,&motorR);
        T0CON0bits.T0EN=1;	//start the timer
        (colorCurrent.clear) = color_read_Clear();
        float current = colorCurrent.clear;
        float maximum = colorCalibration.clear;
        float clear_norm = current/maximum; //normalises clear value depending on calibration routine
        
        //when clear above a certain threshold, start the colour detection and movement process
        if (clear_norm > 0.3){  //normalised clear value range for colour detection
            T0CON0bits.T0EN=0;	//stopping the timer when it reads a value
            int timeF = get16bitTMR0val(); //reads the 16 bit timer value
            logAction('F',timeF);
            stop(&motorL,&motorR);
            hue = reading_hue(&colorCurrent);
            decision(hue);
        }

            
    }

}