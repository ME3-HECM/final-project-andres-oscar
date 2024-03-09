#include <xc.h>
#include "color.h"
#include "i2c.h"
#include "serial.h"

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

void test(unsigned int battery_level) {
    // Assuming LATGbits.LATG0, LATEbits.LATE7, and LATAbits.LATA3 are directly writable and affect the LED states.
    // This loop iterates through all combinations from 0 (000 in binary) to 7 (111 in binary)
    for (unsigned int combo = 0; combo < 8; ++combo) {
        // Set each LED based on the corresponding bit in 'combo'
        LATGbits.LATG0 = (combo & 0x1) ? 1 : 0; // If the first bit is set, turn on the Red LED
        LATEbits.LATE7 = (combo & 0x2) ? 1 : 0; // If the second bit is set, turn on the Green LED
        LATAbits.LATA3 = (combo & 0x4) ? 1 : 0; // If the third bit is set, turn on the Blue LED
        
        // Read color values
        unsigned int red = color_read_Red();
        unsigned int blue = color_read_Blue();
        unsigned int green = color_read_Green();
        unsigned int clear = color_read_Clear();
        
        // Prepare strings for serial transmission
        char red_bit[50];
        char green_bit[50];
        char blue_bit[50];
        
        sprintf(red_bit,"Red_light=%d,  ", LATGbits.LATG0); 
        sprintf(green_bit,"Green_light=%d,  ", LATEbits.LATE7); 
        sprintf(blue_bit,"Blue_light=%d \n\r", LATAbits.LATA3); 
        
        // Send color states and readings over serial
        sendStringSerial4(red_bit);
        sendStringSerial4(green_bit);
        sendStringSerial4(blue_bit);
        
        send2USART(battery_level, red, green, blue, clear);
    }
}