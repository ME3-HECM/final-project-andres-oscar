#include <xc.h>
#include <math.h>
#include "color.h"
#include "i2c.h"
#include "serial.h"
#include "dc_motor.h"

void color_click_init(void)
{   
    //setup colour sensor via i2c interface
    I2C_2_Master_Init();      //Initialise i2c Master

     //set device PON
	 color_writetoaddr(0x00, 0x01);
    __delay_ms(3); //need to wait 3ms for everthing to start up
    
    //turn on device ADC
	color_writetoaddr(0x00, 0x03);

    //set integration time
	color_writetoaddr(0x01, 0xD5);
}

void color_writetoaddr(char address, char value){
    I2C_2_Master_Start();         //Start condition
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit device address + Write mode
    I2C_2_Master_Write(0x80 | address);    //command + register address
    I2C_2_Master_Write(value);    
    I2C_2_Master_Stop();          //Stop condition
}

unsigned int color_read_Red(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x16);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Green(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x18);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Blue(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x1A);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}


unsigned int color_read_Clear(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x14);    //command (auto-increment protocol transaction) + start at CLEAR low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Clear LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Clear MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}


unsigned int reading_hue(colors *cCurr) 
{
    unsigned int hue;
     // Prepare strings for serial transmission
    char led_state[50];

    

        // Turn on Red LED only
    LATGbits.LATG0 = 1; // Red LED on
    LATEbits.LATE7 = 0; // Green LED off
    LATAbits.LATA3 = 0; // Blue LED off
    __delay_ms(500);
    (cCurr->red)= color_read_Red();




    LATGbits.LATG0 = 0; // Red LED off
    LATEbits.LATE7 = 1; // Green LED on
    LATAbits.LATA3 = 0; // Blue LED off   
    __delay_ms(500);
    (cCurr->green) = color_read_Green();





    LATGbits.LATG0 = 0; // Red LED off
    LATEbits.LATE7 = 0; // Green LED off
    LATAbits.LATA3 = 1; // Blue LED on 
    __delay_ms(500);
    (cCurr->blue) = color_read_Blue();




    LATGbits.LATG0 = 1; // Red LED on
    LATEbits.LATE7 = 1; // Green LED on
    LATAbits.LATA3 = 1; // Blue LED on
    __delay_ms(500);
    (cCurr->clear) = color_read_Clear();






    hue = convert_rgb2hue(&colorCalibration, &colorCurrent);




    //send2USART(hue);

    return hue;
    ;

}


void calibration_routine(struct colors *cCal)
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

    
    
    sprintf(cal_state,"CALIBRATION COMPLETED \n\r", ".");
    sendStringSerial4(&cal_state);
    
    while(PORTFbits.RF2 == 1){ //waiting for a last click before leaving function
        
    }
    __delay_ms(500); //small delay to avoid bumping
}


unsigned int convert_rgb2hue(struct colors *cMax, struct colors *cCurr)
{
    float redcurrent, redmax, greencurrent, greenmax, bluecurrent, bluemax, clearcurrent, clearmax;
    float r, g, b, c, min, max, diff, hue = 0.0;
    
    redcurrent = cCurr->red;
    redmax = cMax->red;
    greencurrent = cCurr->green;
    greenmax = cMax->green;
    bluecurrent =  cCurr->blue;
    bluemax = cMax->blue;
    clearcurrent = cCurr->clear;
    clearmax = cMax->clear;
    
    c = clearcurrent/clearmax;
    char c_char[20];
//    sprintf(c_char, "clear=%03d", c);
//    sendStringSerial4(c_char);
    
            
    
    r = redcurrent/redmax;
    g = greencurrent/greenmax;
    b = bluecurrent/bluemax;

    min = r < g ? (r < b ? r : b) : (g < b ? g : b); // Correct calculation of min
    max = r > g ? (r > b ? r : b) : (g > b ? g : b); // Adding calculation of max for completeness
    diff = max - min;
    if (max == min) {
        hue = 0; // If max and min are equal, hue is undefined, usually set to 0
    } else if (max == r){
        hue = (b - g) / diff;
    } else if (max == g){ // Corrected logical AND
        hue = (b - r) / diff;
        hue = hue + 2;
    } else if (max == b){ // Corrected logical AND
        hue = (r - g) / diff;
        hue = hue + 4;
    }

    hue = hue * 60;
    if (hue < 0) {
        hue = hue + 360;
    }
    
    return (unsigned int)hue;
}

void decision(unsigned int hue, unsigned int path_length) {
    // Assume `colorCurrent` holds the latest colors sensor readings
    // and `colorCalibration` holds the calibration data.

    
    unsigned int color;
    
    if (hue<=10 || hue>=355) { // Red hue range
        moveRed(&motorL, &motorR, path_length, &path);
        color = 1;
    } else if (hue>=105 && hue<=130){ // Green hue range
        moveGreen(&motorL, &motorR, path_length, &path);
        color = 2;
    } else if (hue>=230 && hue<=240){ // Blue hue range
        moveBlue(&motorL,&motorR, path_length, &path);
        color = 3;
    } else if (hue>=216 && hue<=221){ // Light Blue hue range
        moveLightBlue(&motorL,&motorR, path_length, &path);
        color = 4;
    } else if (hue>=302 && hue<=346){ // Light Blue hue range
        moveYellow(&motorL,&motorR, path_length, &path);
        color = 5;
    } else if (hue>14 && hue<=35){ // Light Blue hue range
        moveOrange(&motorL,&motorR, path_length, &path);
        color= 6;
    } else if (hue>=244 && hue<=251){ // Light Blue hue range
        movePink(&motorL,&motorR, path_length, &path);  
        color = 7;

    }
    send2USART(color);


    // Here, you can add additional logic to act upon the color detection,
    // such as controlling LEDs or other outputs.
}