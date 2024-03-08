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
    
    
    //finding current battery value, max 255 (we think), so putting it in infinite while loop with LED indicating low battery

    
    unsigned int battery_level;
    unsigned int red;
    unsigned int blue;
    unsigned int green;
    unsigned int clear;
    
    char buf[50];
    char red_char[50];
    char blue_char[50];
    char green_char[50];
    char clear_char[50];
    
    while (1) {
        battery_level = ADC_getval();
        //while (battery_level < 50) {LATDbits.LATD7 = 1;}
        red = color_read_Red();
        blue = color_read_Blue();
        green = color_read_Green();
        clear = color_read_Clear();
        
        //convert values to strings
        ADC2String(buf, battery_level);
        sprintf(red_char,"Red=%d,  ",red); //stores both separate parts in buf
        sprintf(blue_char,"Blue=%d,  ",blue); //stores both separate parts in buf
        sprintf(green_char,"Green=%d,  ",green); //stores both separate parts in buf
        sprintf(clear_char,"Clear=%d,  \r\n",clear); //stores both separate parts in buf

        //send the strings over USART
        //write2USART(buf, red_char, blue_char, green_char, clear_char);
        sendStringSerial4(buf); //Send ADC VAL to realterm program
        sendStringSerial4(red_char);
        sendStringSerial4(blue_char);
        sendStringSerial4(green_char);
        sendStringSerial4(clear_char);
        //square(1);
        
        
    }
}