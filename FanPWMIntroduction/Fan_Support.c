#include <p18f4620.h>
#include "Main.h"
#include "Fan_Support.h"
#include "stdio.h"

extern char FAN;
extern char duty_cycle;
char led1[8] = {0x00,0x04,0x08,0x0c,0x10,0x14,0x18,0x1c} ; 
char led2[31]= {0x00,0x10,0x10,0x10,0x10,0x30,0x30,0x30,0x30,0x30,0x20,0x20,0x20,0x20,0x20,0x40,0x40,0x40,0x40,0x40,0x50,0x50,0x50,0x50,0x50,0x60,0x60,0x60,0x60,0x60,0x70}; //array for LEd 2

int get_RPM()
{
 int RPS = TMR3L / 2; // read the count. Since there are 2 pulses per rev
 // then RPS = count /2
TMR3L = 0; // clear out the count
return (RPS * 60); // return RPM = 60 * RPS 

}

void Toggle_Fan()
{
    if (FAN == 1)
    {
        Turn_Off_Fan();
    }
    else
    {
        Turn_On_Fan();
    }
 
}

void Turn_Off_Fan()
{
    FAN = 0;
    FAN_EN = 0;
}

void Turn_On_Fan()
{
    FAN = 1;
    do_update_pwm(duty_cycle);
    FAN_EN =1;
}

void Increase_Speed()
{
    if (duty_cycle == 100)
    {
        Do_Beep();
        do_update_pwm(duty_cycle);
    }
    else
    {
        duty_cycle = duty_cycle + 5;
        do_update_pwm(duty_cycle);
    }
}

void Decrease_Speed()
{
    if(duty_cycle == 0)
    {
        Do_Beep();
        do_update_pwm(duty_cycle);
    }
    else
    {
        duty_cycle  = duty_cycle-5;
        do_update_pwm(duty_cycle);
    }
 
}

void do_update_pwm(char duty_cycle) 
{ 
    
float dc_f;
 int dc_I;
PR2 = 0b00000100 ; // set the frequency for 25 Khz
T2CON = 0b00000111 ; //
dc_f = ( 4.0 * duty_cycle / 20.0) ; // calculate factor of duty cycle versus a 25 Khz
 // signal
dc_I = (int) dc_f; // get the integer part
 if (dc_I > duty_cycle) dc_I++; // round up function
 CCP1CON = ((dc_I & 0x03) << 4) | 0b00001100;
 CCPR1L = (dc_I) >> 2; 

}

void Set_DC_RGB(int duty_cycle)
{
  
    int led1val = duty_cycle / 10; //get the 10s digit by dividing by 10
    if (led1val > 7 ) //if greater than 7 set to white
    {
        PORTAbits.RA4 =1, PORTAbits.RA3 =1, PORTAbits.RA2 =1; 
        
    }
    switch (led1val) //switch case
    {
        case 0:
            PORTAbits.RA4 = 0, PORTAbits.RA3 =0, PORTAbits.RA2 = 0; //black if 0
            break;
        case 1:
            PORTAbits.RA4 = 0,PORTAbits.RA3 = 0, PORTAbits.RA2 = 1; //red if in the 10s
            break;
        case 2:
            PORTAbits.RA4 = 0,PORTAbits.RA3 = 1, PORTAbits.RA2 =0; //Green if in the 20s
            break;
        case 3:
            PORTAbits.RA4 = 0, PORTAbits.RA3 = 1, PORTAbits.RA2 = 1; //yellow if in the 30s
            break;
        case 4:
            PORTAbits.RA4 = 1, PORTAbits.RA3 = 0, PORTAbits.RA2 = 0; //blue if in the 40s
            break;
        case 5:
            PORTAbits.RA4 = 1, PORTAbits.RA3 = 0, PORTAbits.RA2 = 1; //purple if in the 50s
            break;
        case 6: 
            PORTAbits.RA4 = 1, PORTAbits.RA3 = 1, PORTAbits.RA2 = 0; //cyan if in the 60s
            break;
        case 7:
            PORTAbits.RA4 = 1, PORTAbits.RA3 = 1, PORTAbits.RA2 = 1; //white if in the 70s
    }
}

void Set_RPM_RGB(int rpm)
{
    int led2val = (rpm /100);//divide by 100
    if(led2val > 30) //if rpm > 3000 set to white
    {
           PORTB =( led2[30] | PORTBbits.RB3 | PORTBbits.RB0 );
    
    }
    else{
    PORTB =( led2[led2val] | PORTBbits.RB3 | PORTBbits.RB0 ); //else go through the ray and dont touch other bits
    }
}



