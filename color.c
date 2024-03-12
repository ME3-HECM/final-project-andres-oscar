#include <xc.h>
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

//unsigned int color_decide(Red,Green,Blue)
//{
//    unsigned int color;
//    if (Red>Green && Red>Blue){color = 1;} //Red is dominant color
//    else if (Green>Red && Green>Blue){color = 2;} //Green is dominant color
//    else if (Blue>Red && Blue>Green){color = 3;} //Blue is dominant color
//    
//    return color;
//        
//}

unsigned int convert_rgb2hue(struct colors *cMax, struct colors *cCurr)
{
    float r, g, b, min, max, diff, hue = 0.0;
//    unsigned long long total = ((unsigned long long)cCurr->red * cMax->blue * cMax->green) + ((unsigned long long)cCurr->blue * cMax->red * cMax->green) + ((unsigned long long)cCurr->green * cMax->blue * cMax->red);

    
//    if (total > 0) { // Prevent division by zero
//        r = ((unsigned long long)cCurr->red * cMax->blue * cMax->green) * 100 / total;
//        g = ((unsigned long long)cCurr->green * cMax->blue * cMax->red) * 100 / total;
//        b = 100 - r - g;
//    }
    
    r = cCurr->red / cMax->red;
    g = cCurr->green / cMax->green;
    b = cCurr->blue / cMax->blue;

    

//    char red_weighted[20];
//    char green_weighted[20];
//    char blue_weighted[20];

//    sprintf(red_weighted, "red_weighted =%03d", r);
//    sprintf(green_weighted, "green_weighted =%03d", g);
//    sprintf(blue_weighted, "blue_weighted =%03d", b);
//    sendStringSerial4(red_weighted);
//    sendStringSerial4(green_weighted);
//    sendStringSerial4(blue_weighted);

    min = r < g ? (r < b ? r : b) : (g < b ? g : b); // Correct calculation of min
    max = r > g ? (r > b ? r : b) : (g > b ? g : b); // Adding calculation of max for completeness
    diff = max - min;
    if (max == min) {
        hue = 0; // If max and min are equal, hue is undefined, usually set to 0
    } else {
        if (max == r){
            hue = (b - g) / diff;
        }
        if (g > r && g > b) { // Corrected logical AND
            
            hue = (b - r) / diff;
            hue = hue + 2;
        }
        if (b > r && b > g) { // Corrected logical AND
            diff = b - min;
            hue = (r - g) / diff;
            hue = hue + 4;
        }

        hue = hue * 60;
        if (hue < 0) {
            hue = hue + 360;
        }
    }
    return (unsigned int)hue;
}

    

void test(void) 
{
    unsigned int hue;
     // Prepare strings for serial transmission
    char led_state[50];

    
    for (unsigned int combo = 0; combo < 4; ++combo) {
   
        // Check for specific combos and set LEDs accordingly
        if (combo == 0) {
            // Turn on Red LED only
            LATGbits.LATG0 = 1; // Red LED on
            LATEbits.LATE7 = 0; // Green LED off
            LATAbits.LATA3 = 0; // Blue LED off
            sprintf(led_state,"Red_light=%d, \n\r", LATGbits.LATG0); 


        } 
        
        if (combo == 1) {
            LATGbits.LATG0 = 0; // Red LED off
            LATEbits.LATE7 = 1; // Green LED on
            LATAbits.LATA3 = 0; // Blue LED off   
            sprintf(led_state,"Green_light=%d, \n\r", LATEbits.LATE7); 

        }
        
        if (combo == 2) {
            LATGbits.LATG0 = 0; // Red LED off
            LATEbits.LATE7 = 0; // Green LED off
            LATAbits.LATA3 = 1; // Blue LED on 
            sprintf(led_state,"Blue_light=%d \n\r", LATAbits.LATA3); 


        
        }
        
        if (combo == 3) {
            LATGbits.LATG0 = 1; // Red LED on
            LATEbits.LATE7 = 1; // Green LED on
            LATAbits.LATA3 = 1; // Blue LED on
            sprintf(led_state,"All_lights=%d \n\r", 1);             
        }
        
        sendStringSerial4(led_state);

        reading_values(&colorCurrent);
        
        hue = convert_rgb2hue(&colorCalibration, &colorCurrent);
        
                

       

        send2USART(hue);
        
        
        __delay_ms(500);

    }
}

void calibration_routine(colors *cCal)
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
    
    
    sprintf(cal_state,"Calibration state = ambient light", ".");
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
}

void reading_values(colors *cCurr)
{
    
    // Read color values
    (cCurr->red)= color_read_Red();
    (cCurr->blue) = color_read_Blue();
    (cCurr->green) = color_read_Green();
    (cCurr->clear) = color_read_Clear();
}

void decision(colors *cCurr)
{
    if (cCurr->blue <5 & cCurr->green <5){
        moveRed(&motorL, &motorR);
    }
}