#include <p18f4620.h>
#include "Main.h"
#include "Utils.h"
#include "stdio.h"

void Do_Beep()
{
    Activate_Buzzer(); //Turn the buzzer on
    Wait_One_Sec(); //Wait one second
    Deactivate_Buzzer(); //turn the buzzer off
}

void Wait_One_Sec()
{
    for (int k=0;k<17000;k++); //wait one second via a loop
}

void Activate_Buzzer() //turn the buzzer on using registers calculated via website
{
PR2 = 0b11111001 ;
T2CON = 0b00000101 ;
CCPR2L = 0b01001010 ;
CCP2CON = 0b00111100 ;
}

void Deactivate_Buzzer() //deactivate the buzzer by turning off the port and clearing ccp2con register.
{
CCP2CON = 0x0;
 PORTBbits.RB3 = 0;
}

