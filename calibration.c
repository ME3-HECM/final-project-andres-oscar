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
   

    //wait until button is un pressed
    while(PORTFbits.RF2 == 1){
        
    }
    
    LATGbits.LATG0 = 1; // Red LED on
    __delay_ms(500);
    (cCal->red) = color_read_Red();
    LATGbits.LATG0 = 0; // Red LED off

 
    
    while(PORTFbits.RF2 == 1){
        
    }
    
    LATEbits.LATE7 = 1; // Green LED on
    __delay_ms(500);
    (cCal->green) = color_read_Green();
    LATEbits.LATE7 = 0; // Green LED off


    
    while(PORTFbits.RF2 == 1){
        
    }
    
    LATAbits.LATA3 = 1; // Blue LED on
    __delay_ms(500);
    (cCal->blue) = color_read_Blue();
    LATAbits.LATA3 = 0; // Green LED on
    

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

   

    while(PORTFbits.RF2 == 1){ //waiting for a last click before leaving function
        
    }
    __delay_ms(500); //small delay to avoid bumping
}

void buttons_init(void){
    // setup pin for input (connected to button)
    TRISFbits.TRISF2=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin 
    TRISFbits.TRISF3=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF3=0; //turn off analogue input on pin    
}

unsigned int calibration_turningR(struct DC_motor *mL, struct DC_motor *mR) {
    int factorR = 0;
    float adjustment = 10; // Adjustment rate for the factor.
    unsigned long inactivityCounter = 0;
    const unsigned long inactivityLimit = 3000 / 1; // 5 seconds divided by the delay in the loop below (1ms)

    while (1) {
        // Perform calibration turns.
        for (int i = 0; i < 4; i++) {
            right90(mL, mR, factorR);
            __delay_ms(1000); // Wait a bit between turns for observation.
        }

        inactivityCounter = 0; // Reset inactivity counter after turns.

        // Wait for button input or inactivity timeout.
        while (1) {
            if (!PORTFbits.RF2 || !PORTFbits.RF3) { // If any button is pressed.
                inactivityCounter = 0; // Reset inactivity counter on button press.
            } else {
                inactivityCounter++;
                if (inactivityCounter > inactivityLimit) {
                    // If inactivity timeout is reached, exit function.
                    return (unsigned int)factorR;
                }
            }
            __delay_ms(1); // Delay for inactivity checking.

            // Increase factor with Button 1 (RF2).
            if (PORTFbits.RF2 == 0) {
                __delay_ms(200); // Debounce delay.
                factorR += adjustment;
                while (PORTFbits.RF2 == 0); // Wait until button is released.
                __delay_ms(200); // Further debounce after release.
                break; // Break the inner while loop to reset the process.
            }

            // Decrease factor with Button 2 (RF3).
            if (PORTFbits.RF3 == 0) {
                __delay_ms(200); // Debounce delay.
                factorR -= adjustment;
                while (PORTFbits.RF3 == 0); // Wait until button is released.
                __delay_ms(200); // Further debounce after release.
                break; // Break the inner while loop to reset the process.
            }
        }
    }

}

unsigned int calibration_turningL(struct DC_motor *mL, struct DC_motor *mR) {
    int factorL = 0;
    float adjustment = 10; // Adjustment rate for the factor.
    unsigned long inactivityCounter = 0;
    const unsigned long inactivityLimit = 5000 / 1; // 5 seconds divided by the delay in the loop below (1ms)

    while (1) {
        // Perform calibration turns.
        for (int i = 0; i < 4; i++) {
            left90(mL, mR, factorL);
            __delay_ms(1000); // Wait a bit between turns for observation.
        }

        inactivityCounter = 0; // Reset inactivity counter after turns.

        // Wait for button input or inactivity timeout.
        while (1) {
            if (!PORTFbits.RF2 || !PORTFbits.RF3) { // If any button is pressed.
                inactivityCounter = 0; // Reset inactivity counter on button press.
            } else {
                inactivityCounter++;
                if (inactivityCounter > inactivityLimit) {
                    // If inactivity timeout is reached, exit function.
                    return (unsigned int)factorL;
                }
            }
            __delay_ms(1); // Delay for inactivity checking.

            // Increase factor with Button 1 (RF2).
            if (PORTFbits.RF2 == 0) {
                __delay_ms(200); // Debounce delay.
                factorL += adjustment;
                while (PORTFbits.RF2 == 0); // Wait until button is released.
                __delay_ms(200); // Further debounce after release.
                break; // Break the inner while loop to reset the process.
            }

            // Decrease factor with Button 2 (RF3).
            if (PORTFbits.RF3 == 0) {
                __delay_ms(200); // Debounce delay.
                factorL -= adjustment;
                while (PORTFbits.RF3 == 0); // Wait until button is released.
                __delay_ms(200); // Further debounce after release.
                break; // Break the inner while loop to reset the process.
            }
        }
    }

}
    



//custom delay function for the custom delays when returning
void customDelayMs(unsigned int milliseconds) {
    for (unsigned int i = 0; i < milliseconds; i++) {
        __delay_ms(1); // Delay of 1 ms
    }
}
