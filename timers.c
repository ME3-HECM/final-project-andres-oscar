#include <xc.h>
#include "timers.h"
#include "dc_motor.h"
#include "return_func.h"
/************************************
 * Function to set up timer 0
************************************/
void Timer0_init(void)
{
    T0CON1bits.T0CS=0b010; // Fosc/4
    T0CON1bits.T0ASYNC=1; // see datasheet errata - needed to ensure correct operation when Fosc/4 used as clock source
    T0CON1bits.T0CKPS=0b1101; // 1:8192 - overflows every 33.55 seconds
    T0CON0bits.T016BIT=1;	//16bit mode	
	
    PIE0bits.TMR0IE = 1; //enable intrerrupt
    PIR0bits.TMR0IF = 0; // clear interrupt flag
    IPR0bits.TMR0IP = 0; // low priority
    INTCONbits.IPEN = 1; // enable interrupt priority
    INTCONbits.PEIE = 1; // peripheral interrupt
    INTCONbits.GIE = 1; // global interrupt
    
    TRISGbits.TRISG1 = 1;
    LATGbits.LATG1 = 0;
      
    TMR0L = 0;
    TMR0H = 0;
    
}

/************************************
 * Function to return the full 16bit timer value
 * Note TMR0L and TMR0H must be read in the correct order, or TMR0H will not contain the correct value
************************************/
unsigned int get16bitTMR0val(unsigned int path_step)
{
    int combined_value;
    combined_value = TMR0L | (TMR0H << 8);
    long time_ms = combined_value*65535*4*8192/64000000; // Assuming this is already in milliseconds or calculate correctly.

    logAction(0,time_ms, path_step);
    path_step++;
            
    return path_step;
}
void __interrupt(low_priority) LowISR()
{   
    //toggles bit on board on, to show that it needs to go home and abondon search
    if (PIR0bits.TMR0IF == 1) { // check interrupt flag
        LATGbits.LATG1 = 1;
        
        PIR0bits.TMR0IF = 0; // clear interrupt flag
    }
}