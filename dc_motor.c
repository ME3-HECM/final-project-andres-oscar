#include <xc.h>
#include "dc_motor.h"

// function initialise T2 and CCP for DC motor control
void initDCmotorsPWM(unsigned int PWMperiod){
    //initialise your TRIS and LAT registers for PWM  
    TRISEbits.TRISE2=0; //output on RE2
    TRISCbits.TRISC7=0; //output on RC7
    TRISEbits.TRISE4=0; //output on RE2
    TRISGbits.TRISG6=0; //output on RC7
    
    LATEbits.LATE2=0; // 0 output on RE2
    LATCbits.LATC7=0; // 0 output on RC7
    LATEbits.LATE4=0; // 0 output on RE4
    LATGbits.LATG6=0; // 0 output on RG6
    
    //configure PPS to map CCP modules to pins
    RE2PPS=0x05; //CCP1 on RE2
    RE4PPS=0x06; //CCP2 on RE4
    RC7PPS=0x07; //CCP3 on RC7
    RG6PPS=0x08; //CCP4 on RG6

    // timer 2 config
    T2CONbits.CKPS=0b100; // 1:64 prescaler
    T2HLTbits.MODE=0b00000; // Free Running Mode, software gate only
    T2CLKCONbits.CS=0b0001; // Fosc/4

    // Tpwm*(Fosc/4)/prescaler - 1 = PTPER
    // 0.0001s*16MHz/16 -1 = 99
    T2PR=PWMperiod; //Period reg 10kHz base period
    T2CONbits.ON=1;
    
    //setup CCP modules to output PMW signals
    //initial duty cycles 
    CCPR1H=0; 
    CCPR2H=0; 
    CCPR3H=0; 
    CCPR4H=0; 
    
    //use tmr2 for all CCP modules used
    CCPTMRS0bits.C1TSEL=0;
    CCPTMRS0bits.C2TSEL=0;
    CCPTMRS0bits.C3TSEL=0;
    CCPTMRS0bits.C4TSEL=0;
    
    //configure each CCP
    CCP1CONbits.FMT=1; // left aligned duty cycle (we can just use high byte)
    CCP1CONbits.CCP1MODE=0b1100; //PWM mode  
    CCP1CONbits.EN=1; //turn on
    
    CCP2CONbits.FMT=1; // left aligned
    CCP2CONbits.CCP2MODE=0b1100; //PWM mode  
    CCP2CONbits.EN=1; //turn on
    
    CCP3CONbits.FMT=1; // left aligned
    CCP3CONbits.CCP3MODE=0b1100; //PWM mode  
    CCP3CONbits.EN=1; //turn on
    
    CCP4CONbits.FMT=1; // left aligned
    CCP4CONbits.CCP4MODE=0b1100; //PWM mode  
    CCP4CONbits.EN=1; //turn on
}

// function to set CCP PWM output from the values in the motor structure
void setMotorPWM(DC_motor *m)
{
    unsigned char posDuty, negDuty; //duty cycle values for different sides of the motor
    
    if(m->brakemode) {
        posDuty=m->PWMperiod - ((unsigned int)(m->power)*(m->PWMperiod))/100; //inverted PWM duty
        negDuty=m->PWMperiod; //other side of motor is high all the time
    }
    else {
        posDuty=0; //other side of motor is low all the time
		negDuty=((unsigned int)(m->power)*(m->PWMperiod))/100; // PWM duty
    }
    
    if (m->direction) {
        *(m->posDutyHighByte)=posDuty;  //assign values to the CCP duty cycle registers
        *(m->negDutyHighByte)=negDuty;       
    } else {
        *(m->posDutyHighByte)=negDuty;  //do it the other way around to change direction
        *(m->negDutyHighByte)=posDuty;
    }
}

//function to stop the robot gradually 
void stop(DC_motor *mL, DC_motor *mR)
{
    //as long as each motor power is greater than 0 it gradually decreases it
    while(mL->power > 0 || mR->power > 0) {
        if (mL->power > 0) mL->power--;
        if (mR->power > 0) mR->power--;
        
        //setting new values to the pins
        setMotorPWM(mL);
        setMotorPWM(mR);

        //add a delay for gradual decrease
        __delay_us(500);
    }
}

//function to make the robot turn left 
void turnLeft(DC_motor *mL, DC_motor *mR)
{
    //setting motor pair directions to turn
    mL->direction = 0; //reverse
    mR->direction = 1; //forward

    setMotorPWM(mL);//set new motor direction 
    setMotorPWM(mR);
    for (unsigned int i = 0; i <50; i++)//increase power value up to 50 (this value is tunable for how fast you want it to go)
    {
        mL->power++; //increase the power to both motors slowly
        mR->power++;
        setMotorPWM(mL);//set new motor power values
        setMotorPWM(mR);
        __delay_us(500);//delay to increase power slowly
    } 
    
}

//function to make the robot turn right 
void turnRight(DC_motor *mL, DC_motor *mR)
{
    //setting motor pair directions to turn
    mL->direction = 1; //forward
    mR->direction = 0; //reverse

    setMotorPWM(mL);//set new motor direction 
    setMotorPWM(mR);
    for (unsigned int i = 0; i <50; i++)//increase power value up to 50 (this value is tunable for how fast you want it to go)
    {
        mL->power++; //increase the power to both motors slowly
        mR->power++;
        setMotorPWM(mL);//set new motor power values
        setMotorPWM(mR);
        __delay_us(500);//delay to increase power slowly
    } 
}

//function to make the robot go straight
void fullSpeedAhead(DC_motor *mL, DC_motor *mR)
{
    //setting motor pair directions to go straight ahead
    mL->direction = 1; //forward
    mR->direction = 1; //forward

    for(int power = 0; power <= 50; power++) {
        mL->power = power;
        mR->power = power;
        
        //setting new values to the pins
        setMotorPWM(mL);
        setMotorPWM(mR);

        //add a delay for gradual increase
        __delay_us(500);
    }
    
}

//function to make the robot go backwards
void fullSpeedBack(DC_motor *mL, DC_motor *mR)
{
    //setting motor pair directions to go backwards
    mL->direction = 0; //backward
    mR->direction = 0; //backward

    for(int power = 0; power <= 50; power++) {
        mL->power = power;
        mR->power = power;
        
        //setting new values to the pins
        setMotorPWM(mL);
        setMotorPWM(mR);

        //add a delay for gradual increase
        __delay_us(500);
    }
    
}



/************************************
 * #SET MOVEMENT PIECES! **** indicates tuning required!
 * Tuning all in the same place to avoid changing every function separately
************************************/

//function to turn right 90 degrees
void right90(struct DC_motor *mL, struct DC_motor *mR)
{
    turnRight(mL,mR);//set to turning right mode
    __delay_ms(260); //delay for turning right 90 ****(tune here)
    stop(mL,mR);//stops the rotation
}

//function to turn left 90 degrees
void left90(struct DC_motor *mL, struct DC_motor *mR)
{
    turnLeft(mL,mR); //set to turning left mode
    __delay_ms(260); //delay for turning left 90 ****(tune here)
    stop(mL,mR); //stops the rotation
}

//function to turn 180
void turn180(struct DC_motor *mL, struct DC_motor *mR)
{
    turnLeft(mL,mR); //set to turning left mode
    __delay_ms(520); //delay for turning left 180 ****(tune here)
    stop(mL,mR); //stops the rotation
}

//function to turn right 135
void right135(struct DC_motor *mL, struct DC_motor *mR)
{
    turnRight(mL,mR); //set to turning left mode
    __delay_ms(400); //delay for turning right 135 ****(tune here)
    stop(mL,mR); //stops the rotation
}

//function to turn left 135
void left135(struct DC_motor *mL, struct DC_motor *mR)
{
    turnLeft(mL,mR); //set to turning left mode
    __delay_ms(400); //delay for turning left 135 ****(tune here)
    stop(mL,mR); //stops the rotation
}

//function to move backwards half a unit in the maze
void backHalf(struct DC_motor *mL, struct DC_motor *mR)
{
    fullSpeedBack(mL,mR); //set to going backwards mode
    __delay_ms(300); //delay for moving half a unit of maze ****(tune here)
    stop(mL,mR); //stops the rotation
}

//function to move backwards one and a half units (in one go to remove cumulative errors)
void backOneAndHalf(struct DC_motor *mL, struct DC_motor *mR)
{
    fullSpeedBack(mL,mR); //set to going backwards mode
    __delay_ms(1300); //delay for moving half a unit of maze ****(tune here)
    stop(mL,mR); //stops the rotation
}



/************************************
 * #COLOR MOVEMENT FUNCTIONS! for tuning look above at set pieces
************************************/

void moveRed(struct DC_motor *mL, struct DC_motor *mR)
{
    //Move back half a unit and turn right 90
    backHalf(mL,mR); //moving back half a unit
    __delay_ms(500); //delay to slow down potential skidding
    right90(mL,mR); //turning right 90 function
}

void moveGreen(struct DC_motor *mL, struct DC_motor *mR)
{
    //Move back half a unit and turn left 90
    backHalf(mL,mR); //moving back half a unit
    __delay_ms(500); //delay to slow down potential skidding
    left90(mL,mR); //turning left 90 function
}

void moveBlue(struct DC_motor *mL, struct DC_motor *mR)
{
    //Move back half a unit and turns 180
    backHalf(mL,mR); //moving back half a unit
    __delay_ms(500); //delay to slow down potential skidding
    turn180(mL,mR); //turning 180 function
}

void moveYellow(struct DC_motor *mL, struct DC_motor *mR)
{
    //Move back one and a half units and turn right 90
    backOneAndHalf(mL,mR); //moving back half a unit
    __delay_ms(500); //delay to slow down potential skidding
    right90(mL,mR); //turning right 90 function
}

void movePink(struct DC_motor *mL, struct DC_motor *mR)
{
    //Move back one and a half units and turn left 90
    backOneAndHalf(mL,mR); //moving back half a unit
    __delay_ms(500); //delay to slow down potential skidding
    left90(mL,mR); //turning left 90 function
}

void moveOrange(struct DC_motor *mL, struct DC_motor *mR)
{
    //Move back half a unit and turn right 135
    backHalf(mL,mR); //moving back half a unit
    __delay_ms(500); //delay to slow down potential skidding
    right135(mL,mR); //turning right 135 function
}

void moveLightBlue(struct DC_motor *mL, struct DC_motor *mR)
{
    //Move back half a unit and turn left 135
    backHalf(mL,mR); //moving back half a unit
    __delay_ms(500); //delay to slow down potential skidding
    left135(mL,mR); //turning left 135 function
}

