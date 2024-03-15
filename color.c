#include <xc.h>
#include <math.h>
#include "color.h"
#include "i2c.h"
#include "serial.h"
#include "dc_motor.h"
#include "return_func.h"
#include "timers.h"
#include "maze_navigation.h"

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

void color_clicker_lights_init(void){
        
    //Colored LED initialization
    TRISGbits.TRISG0 = 0;
    LATGbits.LATG0 = 0; //Red is G0
    TRISEbits.TRISE7 = 0;
    LATEbits.LATE7 = 0; //Green is E7
    TRISAbits.TRISA3 = 0;
    LATAbits.LATA3 = 0; //Blue is A3
    
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
    __delay_ms(200);
    (cCurr->red)= color_read_Red();

    LATGbits.LATG0 = 0; // Red LED off
    LATEbits.LATE7 = 1; // Green LED on
    LATAbits.LATA3 = 0; // Blue LED off   
    __delay_ms(200);
    (cCurr->green) = color_read_Green();

    LATGbits.LATG0 = 0; // Red LED off
    LATEbits.LATE7 = 0; // Green LED off
    LATAbits.LATA3 = 1; // Blue LED on 
    __delay_ms(200);
    (cCurr->blue) = color_read_Blue();

    LATGbits.LATG0 = 1; // Red LED on
    LATEbits.LATE7 = 1; // Green LED on
    LATAbits.LATA3 = 1; // Blue LED on
    __delay_ms(200);
    (cCurr->clear) = color_read_Clear();

    
    hue = convert_rgb2hue(&colorCalibration, &colorCurrent);

    //send2USART(hue);

    return hue;
    ;

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

unsigned int decision(unsigned int hue, unsigned int path_step, unsigned int factorR, unsigned int factorL) {
    // Assume `colorCurrent` holds the latest colors sensor readings
    // and `colorCalibration` holds the calibration data.

    
    unsigned int color;
    
    if (hue<=10 || hue>=355) { // Red hue range
        moveRed(&motorL, &motorR, factorR);
        logAction(1,0, path_step); //turning actions have time = 0
        color = 1;
        path_step++; //adds a new step to the path
    }
        else if (hue>=105 && hue<=130){ // Green hue range
        moveGreen(&motorL, &motorR,factorL);
        logAction(2,0, path_step);
        color = 2;
        path_step++; //adds a new step to the path
    } else if (hue>=230 && hue<=248){ // Blue hue range
        moveBlue(&motorL,&motorR);
        logAction(3,0, path_step); //turning actions have time = 0
        color = 3;
        path_step++; //adds a new step to the path
    } else if (hue>=200 && hue<=230 ){ // Light Blue hue range
        moveLightBlue(&motorL,&motorR);
        logAction(4,0, path_step); //turning actions have time = 0
        color = 4;
        path_step++; //adds a new step to the path
    } else if (hue>=302 && hue<=346){ // Light Blue hue range
        moveYellow(&motorL,&motorR, factorR);
        logAction(5,0, path_step); //turning actions have time = 0 CHECK THIS TO SEE IF WE NEED TO REMOVE TIME FROM THE STRAIGHT 
        color = 5;
        path_step++; //adds a new step to the path
    } else if (hue>14 && hue<=35){ // Light Blue hue range
        moveOrange(&motorL,&motorR);
        logAction(6,0, path_step); //turning actions have time = 0
        color= 6;
        path_step++; //adds a new step to the path
    } else if (hue>=240 && hue<=260){ // Light Blue hue range

        movePink(&motorL,&motorR, factorL);  
        logAction(7,0, path_step); //turning actions have time = 0 CHECK THIS TO SEE IF WE NEED TO REMOVE TIME FROM THE STRAIGHT 
        color = 7;
        path_step++; //adds a new step to the path

    }
    //send2USART(color);
    return path_step;
}

unsigned int calc_clear_norm(struct colors *cCurr, struct colors *cMax)
{
    unsigned int clear_norm;
    float clear_max;
    float clear_current;
    colorCurrent.clear = color_read_Clear();
    clear_current = cCurr->clear;
    clear_max = cMax->clear;
    clear_norm = (clear_current)*100/clear_max; 

    return clear_norm;
}


unsigned int is_white(struct DC_motor *mL, struct DC_motor *mR, unsigned int path_step, unsigned int factorR, unsigned int factorL, unsigned int hue, unsigned int clear_norm)
{
    if (((clear_norm > 85 && !(hue >= 302 && hue <= 346)) && !(hue>14 && hue<=35)) || LATGbits.LATG1 == 1) {
        //turn off white light when getting home
        LATGbits.LATG0 = 0; //Red is G0
        LATEbits.LATE7 = 0; //Green is E7
        LATAbits.LATA3 = 0; //Blue is A3

        if (LATGbits.LATG1 == 1){
            get16bitTMR0val(path_step);
            path_step++;
        }
        unsigned int white = 8;
        send2USART(white);
        returnHome(&motorL, &motorR, path_step, factorR, factorL);

    }
}