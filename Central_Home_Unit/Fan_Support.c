
#include <p18f4620.h>
#include "Main.h"
#include "Fan_Support.h"
#include "stdio.h"

extern char HEATER;
extern char duty_cycle;
extern char heater_set_temp;
extern signed int DS1621_tempF;
char led1[8] = {0x00,0x04,0x08,0x0c,0x10,0x14,0x18,0x1c} ;
char led2[31]= {0x00,0x10,0x10,0x10,0x10,0x30,0x30,0x30,0x30,0x30,0x20,0x20,0x20,0x20,0x20,0x40,0x40,0x40,0x40,0x40,0x50,0x50,0x50,0x50,0x50,0x60,0x60,0x60,0x60,0x60,0x70};

int get_duty_cycle(signed int temp, int set_temp)
{
    int diff = set_temp - temp; //The differential is the distance between the two temperatures
    if (diff >=0 && diff < 10) // if the distance between 10, duty cycle is the difference
    {
        duty_cycle = diff;
    }
    else
    {
        if (diff >= 10 && diff < 25) // if differential is between 10 and 25
        {
            duty_cycle = (diff)*(3)/(2); //Duty cycle is 1.5* the differential
        }
        else
        {
            if (diff >=25 && diff < 50) // if differential is between 25 and 50
            {
                duty_cycle = diff * 2; // duty cycle is twice the size of the differential
            }
            else
            {
                if (diff > 50)
                {
                    duty_cycle = 100; //if differential is greater than 50 100% duty cycle
                }
            }
        }
    }
    if (duty_cycle >100) duty_cycle = 100; //if duty cycle attempts to go over 100 correct it back to 100
    if ( duty_cycle < 0) duty_cycle = 0; // if duty cycle attempts to go below 0 set it back to 0
    return duty_cycle;
    
// add code to check if temp is greater than set_temp. If so, set dc according to the handout
// check if dc is greater than 100. If so, set it to 100
// if dc is negative, set to 0
// return dc

}

void Monitor_Heater()
{
    duty_cycle = get_duty_cycle(DS1621_tempF, heater_set_temp);// Get the duty cycle
    do_update_pwm(duty_cycle); //update the PWM with duty cycle

    if (HEATER == 1)  // make sure fan enable is on if heater is on
    {
        FAN_EN = 1;

    }
    else FAN_EN = 0; //if heater is 0 fan enable is off
        
}

void Toggle_Heater()
{
    if (HEATER == 0)//toggle. If the heater is 0 , set to one, and if heater is set to 1 set to 0
    {
        Turn_On_Fan();

    }
    else
    {
        Turn_Off_Fan();
    }
// add code just to flip the variable FAN

}

int get_RPM()
{
int RPS = TMR3L / 2; // read the count. Since there are 2 pulses per rev
 // then RPS = count /2
TMR3L = 0; // clear out the count
return (RPS * 60); // return RPM = 60 * RPS 

// add old code
}

void Turn_Off_Fan()
{
    HEATER = 0; //turn off the heater variable
    FAN_EN = 0; //disable the fan enable

// add old code   

}

void Turn_On_Fan()
{
    HEATER = 1; //turn heater to 1 
    do_update_pwm(duty_cycle); //update the duty cycle
    FAN_EN =1; //enable the fan

}

void Set_DC_RGB(char dC)
{
  
    int led1val = dC / 10; //divide by 10 to get the coefficient
    if (led1val > 7 ) // if greater than 7 set tow hite
    {
        PORTAbits.RA4 =1, PORTAbits.RA3 =1, PORTAbits.RA2 =1;
        
    }
    switch (led1val) //switch/case statement so as not to ruin other pins on the port
    {
        case 0:
            PORTAbits.RA4 = 0, PORTAbits.RA3 =0, PORTAbits.RA2 = 0; //off 
            break;
        case 1:
            PORTAbits.RA4 = 0,PORTAbits.RA3 = 0, PORTAbits.RA2 = 1; //red
            break;
        case 2:
            PORTAbits.RA4 = 0,PORTAbits.RA3 = 1, PORTAbits.RA2 =0; //green
            break;
        case 3:
            PORTAbits.RA4 = 0, PORTAbits.RA3 = 1, PORTAbits.RA2 = 1;//yellow
            break;
        case 4:
            PORTAbits.RA4 = 1, PORTAbits.RA3 = 0, PORTAbits.RA2 = 0; //b;ue
            break;
        case 5:
            PORTAbits.RA4 = 1, PORTAbits.RA3 = 0, PORTAbits.RA2 = 1; //purple
            break;
        case 6: 
            PORTAbits.RA4 = 1, PORTAbits.RA3 = 1, PORTAbits.RA2 = 0; //cyan
            break;
        case 7:
            PORTAbits.RA4 = 1, PORTAbits.RA3 = 1, PORTAbits.RA2 = 1; //white
    }
}

void Set_RPM_RGB(int rpm)
{
    int led2val = (rpm /100);
    if(led2val > 30)
    {
           PORTB =( led2[30] | PORTBbits.RB3 | PORTBbits.RB0 );
    
    }
    else{
    PORTB =( led2[led2val] | PORTBbits.RB3 | PORTBbits.RB0 );
    }
}







