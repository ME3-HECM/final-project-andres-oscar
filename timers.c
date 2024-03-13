#include <xc.h>
#include "timers.h"

/************************************
 * Function to set up timer 0
************************************/
void Timer0_init(void)
{
    T0CON1bits.T0CS=0b010; // Fosc/4
    T0CON1bits.T0ASYNC=1; // see datasheet errata - needed to ensure correct operation when Fosc/4 used as clock source
    T0CON1bits.T0CKPS=0b1000; // 1:225
    T0CON0bits.T016BIT=1;	//16bit mode	
	
    PIE0bits.TMR0IE = 1; //enable intrerrupt
    PIR0bits.TMR0IF = 0; // clear interrupt flag
    IPR0bits.TMR0IP = 0; // low priority
    INTCONbits.IPEN = 1; // enable interrupt priority
    INTCONbits.PEIE = 1; // peripheral interrupt
    INTCONbits.GIE = 1; // global interrupt
       
}

/************************************
 * Function to return the full 16bit timer value
 * Note TMR0L and TMR0H must be read in the correct order, or TMR0H will not contain the correct value
************************************/
unsigned int get16bitTMR0val(void)
{
    unsigned int combined_value;
    combined_value = TMR0L | (TMR0H << 8);
    return combined_value;
}
