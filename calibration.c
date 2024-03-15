#include <xc.h>
#include <math.h>
#include "color.h"
#include "i2c.h"
#include "serial.h"
#include "dc_motor.h"
#include "return_func.h"
#include "calibration.h"
void calibration_colors(colors *cCal)
{
  
    LATGbits.LATG0 = 0; // Red LED off
    LATEbits.LATE7 = 0; // Green LED off
    LATAbits.LATA3 = 0; // Green LED off
    
    char cal_state[20];
    
     

    sprintf(cal_state,"Calibration state = red", ".");
    sendStringSerial4(cal_state);

    //wait until button is un pressed
    while(PORTFbits.RF2 == 1){
        
    }
    
    LATGbits.LATG0 = 1; // Red LED on
    __delay_ms(500);
    (cCal->red) = color_read_Red();
    LATGbits.LATG0 = 0; // Red LED off

    
    sprintf(cal_state,"Calibration state = green \n\r", ".");
    sendStringSerial4(&cal_state);
    
    while(PORTFbits.RF2 == 1){
        
    }
    
    LATEbits.LATE7 = 1; // Green LED on
    __delay_ms(500);
    (cCal->green) = color_read_Green();
    LATEbits.LATE7 = 0; // Green LED off

    
    sprintf(cal_state,"Calibration state= blue \n\r", ".");
    sendStringSerial4(&cal_state);
    
    while(PORTFbits.RF2 == 1){
        
    }
    
    LATAbits.LATA3 = 1; // Blue LED on
    __delay_ms(500);
    (cCal->blue) = color_read_Blue();
    LATAbits.LATA3 = 0; // Green LED on
    
    
    sprintf(cal_state,"Calibration state =  white", ".");
    sendStringSerial4(&cal_state);    
    
    while(PORTFbits.RF2 == 1){
        
    }
    LATGbits.LATG0 = 1; // Red LED on
    LATEbits.LATE7 = 1; // Green LED on
    LATAbits.LATA3 = 1; // Blue LED on
    __delay_ms(500);
    (cCal->clear) = color_read_Clear();
    LATGbits.LATG0 = 0; // Red LED off
    LATEbits.LATE7 = 0; // Green LED off
    LATAbits.LATA3 = 0; // Green LED off
    
    sprintf(cal_state,"Calibration state =  ambient", ".");
    sendStringSerial4(&cal_state);    
    
    while(PORTFbits.RF2 == 1){
        
    }
    LATGbits.LATG0 = 1; // Red LED on
    LATEbits.LATE7 = 1; // Green LED on
    LATAbits.LATA3 = 1; // Blue LED on
    __delay_ms(500);
    (cCal->ambient) = color_read_Clear();
    LATGbits.LATG0 = 0; // Red LED off
    LATEbits.LATE7 = 0; // Green LED off
    LATAbits.LATA3 = 0; // Green LED off

    
    
    sprintf(cal_state,"CALIBRATION COMPLETED \n\r", ".");
    sendStringSerial4(&cal_state);
    send2USART(colorCalibration.ambient);

    while(PORTFbits.RF2 == 1){ //waiting for a last click before leaving function
        
    }
    __delay_ms(500); //small delay to avoid bumping
}


unsigned int calibration_turning(struct DC_motor *mL, struct DC_motor *mR){
    float factor = 0.9; // Use float for more precise adjustment
    float adjustment = 0.05; // Adjustment rate

    // Introduce a way to break out of the while loop
    unsigned char exitLoop = 0;

    while(!exitLoop){
        // Increase factor with RF3
        if (PORTFbits.RF2 == 0){
            __delay_ms(200); // Debounce delay
            factor += adjustment;
            while(PORTFbits.RF2 == 0); // Wait until button is released
        }



        // Perform the calibration turns with the final factor
         for(int i = 0; i < 4; i++) {
             right90(mL, mR, factor);
             __delay_ms(300); // Delay between turns for observation
            }
        __delay_ms(1000);
        
        // Check if both buttons are pressed to confirm and exit
        if (PORTFbits.RF3 == 0){
            __delay_ms(200); // Debounce delay
            exitLoop = 1; // Set to exit the loop
            while(PORTFbits.RF3 == 0); // Wait until both buttons are released
        }
        
    }

    

    // Assuming factor needs to be an integer for some reason, otherwise just use float
    return (unsigned int)(factor); // Return factor scaled up
}


    



//custom delay function for the custom delays when returning
void customDelayMs(unsigned int milliseconds) {
    for (unsigned int i = 0; i < milliseconds; i++) {
        __delay_ms(1); // Delay of 1 ms
    }
}
