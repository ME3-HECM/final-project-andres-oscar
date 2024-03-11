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

     // Prepare strings for serial transmission
    char led_state[50];

    
    for (unsigned int combo = 0; combo < 4; ++combo) {
   
        // Check for specific combos and set LEDs accordingly
        if (combo == 0) {
            // Turn on Red LED only
            LATGbits.LATG0 = 1; // Red LED on
            LATEbits.LATE7 = 0; // Green LED on
            LATAbits.LATA3 = 0; // Green LED on
            sprintf(led_state,"Red_light=%d, \n\r", LATGbits.LATG0); 


        } 
        
        if (combo == 1) {
            LATGbits.LATG0 = 0; // Red LED on
            LATEbits.LATE7 = 1; // Green LED on
            LATAbits.LATA3 = 0; // Green LED on    
            sprintf(led_state,"Green_light=%d, \n\r", LATEbits.LATE7); 

        }
        
        if (combo == 2) {
            LATGbits.LATG0 = 0; // Red LED on
            LATEbits.LATE7 = 0; // Green LED on
            LATAbits.LATA3 = 1; // Green LED on 
            sprintf(led_state,"Blue_light=%d \n\r", LATAbits.LATA3); 


        
        }
        
        if (combo == 3) {
            LATGbits.LATG0 = 1; // Red LED on
            LATEbits.LATE7 = 1; // Green LED on
            LATAbits.LATA3 = 1; // Green LED on
            sprintf(led_state,"All_lights=%d \n\r", 1); 

            
        }
        
        sendStringSerial4(led_state);

        // Read color values
        unsigned int red = color_read_Red();
        unsigned int blue = color_read_Blue();
        unsigned int green = color_read_Green();
        unsigned int clear = color_read_Clear();
        
       
        
                

       

        send2USART(battery_level, red, green, blue, clear);
    }
}