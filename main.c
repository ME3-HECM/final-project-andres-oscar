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
    Timer0_init();
    
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
    

    

    
    //Colored LED initialization
    TRISGbits.TRISG0 = 0;
    LATGbits.LATG0 = 0; //Red is G0
    TRISEbits.TRISE7 = 0;
    LATEbits.LATE7 = 0; //Green is E7
    TRISAbits.TRISA3 = 0;
    LATAbits.LATA3 = 0; //Blue is A3
    
    //finding current battery value, max 255 (we think), so putting it in infinite while loop with LED indicating low battery
    
    // setup pin for input (connected to button)
    TRISFbits.TRISF2=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin 
    TRISFbits.TRISF3=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF3=0; //turn off analogue input on pin 
    
  
    

    
    
    //flashes light on the front of car to show start
    TRISHbits.TRISH3 = 0;
    LATHbits.LATH3 = 1;
    __delay_ms(300);
    LATHbits.LATH3 = 0;
    
    
    
    calibration_colors(&colorCalibration);

    unsigned int clear_norm;
    unsigned int path_step = 0;
    unsigned int hue;
    unsigned int ambient;
    float clear_current;
    float clear_max;
    unsigned int factorR;
    unsigned int factorL;

    
    factorR = calibration_turningR(&motorL, &motorR) ;
    factorL = calibration_turningL(&motorL, &motorR) ;
    
    ambient = colorCalibration.ambient;

    send2USART(ambient);
    //code structure for testing the movement functions
    while (1) {
        //turn on white light during normal operation
        LATGbits.LATG0 = 1; //Red is G0
        LATEbits.LATE7 = 1; //Green is E7
        LATAbits.LATA3 = 1; //Blue is A3
        
        //moves forward
        fullSpeedAhead(&motorL,&motorR);
        T0CON0bits.T0EN=1;	//start the timer
    
        colorCurrent.clear = color_read_Clear();

        clear_current = colorCurrent.clear;
        send2USART(clear_current);
        clear_max = colorCalibration.clear;
        //float diff = clear_max - ambient;
        clear_norm = (clear_current)*100/clear_max;    
                
        
        //when clear above a certain threshold, start the colour detection and movement process
        while(clear_norm<10){ //change the addition depending on ambient
            (colorCurrent.clear) = color_read_Clear();
            clear_current = colorCurrent.clear;
            clear_max = colorCalibration.clear;
            clear_norm = clear_current*100/clear_max;   
                
            }
        
        T0CON0bits.T0EN=0;	//stops the timer

        stop(&motorL,&motorR); //stops moving


        path_step = get16bitTMR0val(path_step); //takes the timer value at that instant

        __delay_ms(200);

        //small sequence to bump wall for better readings
        fullSpeedAhead(&motorL,&motorR);
        __delay_ms(300);
        stop(&motorL,&motorR);
        __delay_ms(300);

        hue = reading_hue(&colorCurrent);
        send2USART(hue);
        clear_current = colorCurrent.clear;
        clear_max = colorCalibration.clear;
        clear_norm = clear_current*100/clear_max;   
        send2USART(clear_norm);
                
        if (clear_norm > 20){
            fullSpeedAhead(&motorL,&motorR);
            __delay_ms(100);
            stop(&motorL,&motorR);
        }  
        if ((clear_norm > 85 && !(hue >= 302 && hue <= 346)) && !(hue>14 && hue<=35) || LATGbits.LATG1 == 1) {
                    //turn off white light during normal operation
            LATGbits.LATG0 = 0; //Red is G0
            LATEbits.LATE7 = 0; //Green is E7
            LATAbits.LATA3 = 0; //Blue is A3

            if (LATGbits.LATG1 == 1){
                path_step = get16bitTMR0val(path_step);
            }
            unsigned int white = 8;
            send2USART(white);
            returnHome(&motorL, &motorR, path_step, factorR, factorL);
            
        }
            
        path_step = decision(hue, path_step, factorR, factorL);
    }

            
}

