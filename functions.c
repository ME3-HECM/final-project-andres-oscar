#include <xc.h>
#include "functions.h"
#include "serial.h"

void write2USART(char buf, char red_char, char blue_char, char green_char, char clear_char)
{
    sendStringSerial4(buf); //Send ADC VAL to realterm program
    sendStringSerial4(red_char);
    sendStringSerial4(blue_char);
    sendStringSerial4(green_char);
    sendStringSerial4(clear_char);
}
