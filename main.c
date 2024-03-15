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
    ANSELFbits.ANSELF3=0; 
    TRISFbits.TRISF2 = 1; //another pin input
    LATFbits.LATF2 = 0;
    ANSELFbits.ANSELF2 = 0;
    
    //initialising another pin on the board to hold interrupt value
    TRISGbits.TRISG1 = 0;
    LATGbits.LATG1 = 0;
    

    
    
    //flashes light on the front of car to show initialization
    TRISHbits.TRISH3 = 0;
    LATHbits.LATH3 = 1;
    __delay_ms(300);
    LATHbits.LATH3 = 0;
    
    
    
    calibration_routine(&colorCalibration);
    send2USART(colorCalibration.ambient);
    unsigned int clear_norm;
    float current;
    unsigned int path_step = 0;
    unsigned int hue;
    unsigned int ambient;

    
    


    //code structure for testing the movement functions
    while (1) {
        //turn on white light during normal operation
        LATGbits.LATG0 = 1; //Red is G0
        LATEbits.LATE7 = 1; //Green is E7
        LATAbits.LATA3 = 1; //Blue is A3
        
        //moves forward
        fullSpeedAhead(&motorL,&motorR);
        T0CON0bits.T0EN=1;	//start the timer
    
        (colorCurrent.clear) = color_read_Clear();

 
        clear_norm = (colorCurrent.clear)*100/colorCalibration.clear;    
                
        
        send2USART(clear_norm);
        //when clear above a certain threshold, start the colour detection and movement process
        while(clear_norm<8){
            (colorCurrent.clear) = color_read_Clear();
            clear_norm = (colorCurrent.clear)*100/colorCalibration.clear;    

            send2USART(clear_norm);

        }
       
        T0CON0bits.T0EN=0;	//stop the timer
        stop(&motorL,&motorR); //stops moving


        path_step = get16bitTMR0val(path_step); //takes the timer value at that instant

        __delay_ms(200);

        //small sequence to bump wall for better readings
        fullSpeedAhead(&motorL,&motorR);
        __delay_ms(300);
        stop(&motorL,&motorR);
        __delay_ms(300);

        hue = reading_hue(&colorCurrent);

        clear_norm = (colorCurrent.clear)*100/colorCalibration.clear;    

                
        if (clear_norm > 20){
            fullSpeedAhead(&motorL,&motorR);
            __delay_ms(100);
            stop(&motorL,&motorR);
        }  
        if ((clear_norm > 60 && !(hue >= 302 && hue <= 346)) || LATGbits.LATG1 == 1) {

            unsigned int white = 8;
            send2USART(white);
            returnHome(&motorL, &motorR, path_step);
            LATGbits.LATG1 = 0;
        }
            
        path_step = decision(hue, path_step);
    }

            
}

