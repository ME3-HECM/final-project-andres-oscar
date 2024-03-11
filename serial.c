#include <xc.h>
#include "serial.h"
#include "ADC.h"
#include <stdio.h>



// Global variables
extern volatile char dataFlag; // Flag to indicate data is in the TX buffer
volatile unsigned int ldrReadings[10]; // Buffer to store LDR readings
volatile unsigned char readingIndex = 0; // Index for the next LDR reading to store


void initUSART4(void) {
    RC0PPS = 0x12; // Map EUSART4 TX to RC0
    RX4PPS = 0x11; // RX is RC1   
    BAUD4CONbits.BRG16 = 0; 	//set baud rate scaling
    TX4STAbits.BRGH = 0; 		//high baud rate select bit
    SP4BRGL = 51; 			//set baud rate to 51 = 19200bps
    SP4BRGH = 0;			//not used

    RC4STAbits.CREN = 1; 		//enable continuos reception
    TX4STAbits.TXEN = 1; 		//enable transmitter
    RC4STAbits.SPEN = 1; 		//enable serial port
}


//function to wait for a byte to arrive on serial port and read it once it does 
char getCharSerial4(void) {
    while (!PIR4bits.RC4IF);//wait for the data to arrive
    return RC4REG; //return byte in RCREG
}

//function to check the TX reg is free and send a byte
void sendCharSerial4(char charToSend) {
    while (!PIR4bits.TX4IF); // wait for flag to be set
    TX4REG = charToSend; //transfer char to transmitter
}


void sendStringSerial4(char *string){
	//Hint: look at how you did this for the LCD lab 
    // This counts the amount of spaces used 
    while(*string != 0){  // While the data pointed to isn't a 0x00 do below (strings in C must end with a NULL byte) 
        //Send out the current byte pointed to and increment the pointer
		sendCharSerial4(*string++); 
        __delay_us(20);
	}
}


//functions below are for Ex3 and 4 (optional)

// circular buffer functions for RX
// retrieve a byte from the buffer
char getCharFromRxBuf(void){
    if (RxBufReadCnt>=RX_BUF_SIZE) {RxBufReadCnt=0;} 
    return EUSART4RXbuf[RxBufReadCnt++];
}

// add a byte to the buffer
void putCharToRxBuf(char byte){
    if (RxBufWriteCnt>=RX_BUF_SIZE) {RxBufWriteCnt=0;}
    EUSART4RXbuf[RxBufWriteCnt++]=byte;
}

// function to check if there is data in the RX buffer
// 1: there is data in the buffer
// 0: nothing in the buffer
char isDataInRxBuf (void){
    return (RxBufWriteCnt!=RxBufReadCnt);
}

// circular buffer functions for TX
// retrieve a byte from the buffer
char getCharFromTxBuf(void){
    if (TxBufReadCnt>=TX_BUF_SIZE) {TxBufReadCnt=0;} 
    return EUSART4TXbuf[TxBufReadCnt++];
}

// add a byte to the buffer
void putCharToTxBuf(char byte){
    if (TxBufWriteCnt>=TX_BUF_SIZE) {TxBufWriteCnt=0;}
    EUSART4TXbuf[TxBufWriteCnt++]=byte;
}

// function to check if there is data in the TX buffer
// 1: there is data in the buffer
// 0: nothing in the buffer
char isDataInTxBuf (void){
    return (TxBufWriteCnt!=TxBufReadCnt);
}

//add a string to the buffer
void TxBufferedString(char *string){
	//Hint: look at how you did this for the LCD lab 
    while(*string != 0){  // While the data pointed to isn't a 0x00 do below (strings in C must end with a NULL byte) 
        //Send out the current byte pointed to and increment the pointer
		putCharToTxBuf(*string++); 
    }
}

//initialise interrupt driven transmission of the Tx buf
//your ISR needs to be setup to turn this interrupt off once the buffer is empty
void sendTxBuf(void){
    //enable the TX interrupt to send data
    if (isDataInTxBuf()) {
        PIE4bits.TX4IE=1;
        dataFlag=1;
    }     
    //resets data flag
    else { 
        dataFlag=0;
    }
}


//COULD DELETE THIS?
void sendAllReadings() {
    char buffer[10];
    for (unsigned char i = 0; i < readingIndex; ++i) {
        unsigned int reading = ldrReadings[i];
        // Assuming ADC2String function formats the reading into a string
        //ADC2String(buffer, reading);
        
        //convert ADC value to string
        ADC2String(buffer, reading);
        // Send the formatted string over USART
        sendStringSerial4(buffer);
        // Small delay to ensure data is sent without overflowing UART buffers
        __delay_ms(10);
    }
}

void ADC2String(char *buf, unsigned int ADC_val){
	//code to calculate the integer and fractions part of a ADC value
    unsigned int int_part=ADC_val/77; // i.e. 255 / 3.3V = 77 

    unsigned int frac_part=(ADC_val*100)/77 - int_part*100; //calculates fractional part
    
    sprintf(buf,"Voltage = %d.%02d,  ",int_part,frac_part); //stores both separate parts in buf
    
    __delay_ms(1000);
}

void send2USART(unsigned int battery_level, unsigned int red, unsigned int green, unsigned int  blue, unsigned int clear){
        char buf[50];
        char red_char[50];
        char blue_char[50];
        char green_char[50];
        char clear_char[50];

        
    //convert values to strings
        ADC2String(buf, battery_level);
        sprintf(red_char,"Red=%05d,  ",red); //stores both separate parts in buf
        sprintf(blue_char,"Blue=%05d,  ",blue); //stores both separate parts in buf
        sprintf(green_char,"Green=%05d,  ",green); //stores both separate parts in buf
        sprintf(clear_char,"Clear=%05d,  \n\n\r",clear); //stores both separate parts in buf

        //send the strings over USART
        //write2USART(buf, red_char, blue_char, green_char, clear_char);
        sendStringSerial4(buf); //Send ADC VAL to realterm program
        sendStringSerial4(red_char);
        sendStringSerial4(blue_char);
        sendStringSerial4(green_char);
        sendStringSerial4(clear_char);
}

