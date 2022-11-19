#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

#include <p18f4620.h>
#include "utils.h"
#include "Main.h"
extern char found;
extern char Nec_code1;
extern short nec_ok;
extern char array1[21];
extern char duty_cycle;
char d3rgb[8] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07};
char check_for_button_input()
{       
    if (nec_ok == 1)
    {
            nec_ok = 0;


            INTCONbits.INT0IE = 1;          // Enable external interrupt
            INTCON2bits.INTEDG0 = 0;        // Edge programming for INT0 falling edge

            found = 0xff;
            for (int j=0; j< 21; j++)
            {
                if (Nec_code1 == array1[j]) 
                {
                    found = j;
                    j = 21;
                }
            }
            
            if (found == 0xff) 
            {
                printf ("Cannot find button \r\n");
                return (0);
            }
            else
            {
                return (1);
            }
    }
}

char bcd_2_dec (char bcd)
{
    int dec; //provided by professor
    dec = ((bcd>> 4) * 10) + (bcd & 0x0f);
    return dec;
}

int dec_2_bcd (char dec)
{
    int bcd; //provided by  professor
    bcd = ((dec / 10) << 4) + (dec % 10);
    return bcd;
}

void Do_Beep()
{
    KEY_PRESSED = 1;//turn on the key pressed LED
    Activate_Buzzer(); //Beep 
    Wait_One_Sec(); //wait one second
    KEY_PRESSED = 0; //turn off the Key pressed LED
    Deactivate_Buzzer(); //stop beeping
    do_update_pwm(duty_cycle);//update with duty cycle
}

void Do_Beep_Good()
{
    KEY_PRESSED = 1; //Turn LED on
    Activate_Buzzer_2KHz(); //turn the 2khz buzzer on
    Wait_One_Sec(); //wait 1 second
    KEY_PRESSED = 0; //turn the led off
    Deactivate_Buzzer(); //stop buzzing
    do_update_pwm(duty_cycle); //update with duty cycle
// add code here using Activate_Buzzer_2KHz()
}

void Do_Beep_Bad()
{
    KEY_PRESSED = 1; //Turn LED on
    Activate_Buzzer_500Hz(); //Do the low frequency beep 
    Wait_One_Sec(); //wait one second
    KEY_PRESSED = 0; //Turn the LED off
    Deactivate_Buzzer(); // Stop beeping
    do_update_pwm(duty_cycle); //update with the duty cycle
// add code here using Activate_Buzzer_500Hz()

}

void Wait_One_Sec()
{
    for (int k=0;k<0x8000;k++);
    
}

void Activate_Buzzer() //code provided by professor
{ 
    PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;
    CCPR2L = 0b01001010 ;
    CCP2CON = 0b00111100 ;
}

void Activate_Buzzer_500Hz() //calculated registers using website
{
PR2 = 0b11111001 ;
T2CON = 0b00000111 ;
CCPR2L = 0b11111001 ;
CCP2CON = 0b00111100 ;
    // add code here
}

void Activate_Buzzer_2KHz() //calculated registers using website
{
PR2 = 0b11111001 ;
T2CON = 0b00000101 ;
CCPR2L = 0b11111001 ;
CCP2CON = 0b00111100 ;
    // add code here
}
 
void Activate_Buzzer_4KHz() //calculated registers using website
{
PR2 = 0b01111100 ;
T2CON = 0b00000101 ;
CCPR2L = 0b01111100 ;
CCP2CON = 0b00111100 ;
// add code here
}

void Deactivate_Buzzer() //disable the buzzer 
{ 
    CCP2CON = 0x0;
	PORTBbits.RB3 = 0;
}

void do_update_pwm(char duty_cycle) 
{ 
	float dc_f;
	int dc_I;
	PR2 = 0b00000100 ;                      // Set the frequency for 25 Khz 
	T2CON = 0b00000111 ;                    // As given in website
	dc_f = ( 4.0 * duty_cycle / 20.0) ;     // calculate factor of duty cycle versus a 25 Khz signal
	dc_I = (int) dc_f;                      // Truncate integer
	if (dc_I > duty_cycle) dc_I++;          // Round up function
	CCP1CON = ((dc_I & 0x03) << 4) | 0b00001100;
	CCPR1L = (dc_I) >> 2;
}

void Set_RGB_Color(char color)
{
    PORTE=d3rgb[color]; //go through the RGB array

}

float read_volt()
{
    char ch = 0;
 ADCON0 = ch*4+1; //channel 1
 int nStep = get_full_ADC();
 float Volt = ((nStep*5.0)/1024.0) ; //multiply by 5. Then divide by 1024 due to reference voltage
 return (Volt); //return the voltage
    // add code here
}

unsigned int get_full_ADC()
{
    // add code here
int result;
 ADCON0bits.GO=1; // Start Conversion
 while(ADCON0bits.DONE==1); // Wait for conversion to be completed (DONE=0)
 result = (ADRESH * 0x100) + ADRESL; // Combine result of upper byte and lower byte into
 return result; // return the most significant 8- bits of the result.
}

void Init_ADC()
{
    ADCON0 = 0x01; //ADcontrol registers as set in previous labs
    ADCON1 = 0x0E;
    ADCON2 = 0xA9;
    // add code here
}


