#ifndef _DC_MOTOR_H
#define _DC_MOTOR_H

#include <xc.h>

#define _XTAL_FREQ 64000000
#define MAX_PATH_LENGTH 50  // variable for the maximum length of buggy path


typedef struct DC_motor { //definition of DC_motor structure
    char power;         //motor power, out of 100
    char direction;     //motor direction, forward(1), reverse(0)
    char brakemode;		// short or fast decay (brake or coast)
    unsigned int PWMperiod; //base period of PWM cycle
    unsigned char *posDutyHighByte; //PWM duty address for motor +ve side
    unsigned char *negDutyHighByte; //PWM duty address for motor -ve side
} DC_motor;

struct DC_motor motorL, motorR; 		//declare two DC_motor structures 

typedef struct PathStep{
    unsigned int action;  //numbers corresponding to actions
    int time;     //time duration for the forward action, 0 use'd a's placeholder in turns
    unsigned int path_length;
} PathStep;

struct PathStep path[MAX_PATH_LENGTH]; //declaring the path structure (can store 50 actions)

//function prototypes
void initDCmotorsPWM(unsigned int PWMperiod); // function to setup PWM
void setMotorPWM(DC_motor *m);
void stop(DC_motor *mL, DC_motor *mR);
void turnLeft(DC_motor *mL, DC_motor *mR);
void turnRight(DC_motor *mL, DC_motor *mR);
void fullSpeedAhead(DC_motor *mL, DC_motor *mR);
//set pieces
void right90(struct DC_motor *mL, struct DC_motor *mR);
void left90(struct DC_motor *mL, struct DC_motor *mR);
void turn180(struct DC_motor *mL, struct DC_motor *mR);
void right135(struct DC_motor *mL, struct DC_motor *mR);
void left135(struct DC_motor *mL, struct DC_motor *mR);
void backHalf(struct DC_motor *mL, struct DC_motor *mR);
void backOneAndHalf(struct DC_motor *mL, struct DC_motor *mR);
//color movements
void moveRed(struct DC_motor *mL, struct DC_motor *mR, unsigned int path_length);
void moveGreen(struct DC_motor *mL, struct DC_motor *mR, unsigned int path_length);
void moveBlue(struct DC_motor *mL, struct DC_motor *mR,unsigned int path_length);
void moveYellow(struct DC_motor *mL, struct DC_motor *mR, unsigned int path_length);
void movePink(struct DC_motor *mL, struct DC_motor *mR, unsigned int path_length);
void moveOrange(struct DC_motor *mL, struct DC_motor *mR, unsigned int path_length);
void moveLightBlue(struct DC_motor *mL, struct DC_motor *mR, unsigned int path_length);
void moveWhite(struct DC_motor *mL, struct DC_motor *mR);
//returning related
unsigned int logAction(char action, int time, unsigned int pathLength);
void reverseTurn(struct DC_motor *mL, struct DC_motor *mR, char turnDirection);
void reverseStraight(struct DC_motor *mL, struct DC_motor *mR, int time);
void returnHome(struct DC_motor *mL, struct DC_motor *mR, struct PathStep *path[], int pathLength);

#endif
