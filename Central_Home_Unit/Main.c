#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>
#include <string.h>

#include "I2C_Soft.h"
#include "I2C_Support.h"
#include "Interrupt.h"
#include "Fan_Support.h"
#include "Main.h"
#include "ST7735_TFT.h"
#include "utils.h"
#include "Setup_Alarm_Time.h"
#include "Setup_Time.h"
#include "Setup_Heater_Temp.h"

#pragma config OSC = INTIO67
#pragma config BOREN =OFF
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config CCP2MX = PORTBE

void Test_Alarm();
char second = 0x00;
char minute = 0x00;
char hour = 0x00;
char dow = 0x00;
char day = 0x00;
char month = 0x00;
char year = 0x00;

char found;
char tempSecond = 0xff; 
signed int DS1621_tempC, DS1621_tempF;
char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
char alarm_second, alarm_minute, alarm_hour, alarm_date;
char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;
unsigned char heater_set_temp = 75;
unsigned char Nec_state = 0;
float volt;
char INT1_flag, INT2_flag;

short nec_ok = 0;
char Nec_code1;
char HEATER;
char duty_cycle;
int rps;
int rpm;
int ALARMEN;
int alarm_mode, MATCHED;
int color = 0;

char buffer[31]     = " ECE3301L Sp'22 S4 \0";
char *nbr;
char *txt;
char tempC[]        = "+25";
char tempF[]        = "+77";
char time[]         = "00:00:00";
char date[]         = "00/00/00";
char alarm_time[]   = "00:00:00";
char Alarm_SW_Txt[] = "OFF";
char Heater_Set_Temp_Txt[] = "075F";
char Heater_SW_Txt[]   = "OFF";                // text storage for Heater Mode
char array1[21]={0xa2,0x62,0xe2,0x22,0x02,0xc2,0xe0,0xa8,0x90,0x68,0x98,0xb0,0x30,0x18,0x7a,0x10,0x38,0x5a,0x42,0x4a,0x52};

    
char DC_Txt[]       = "000";                // text storage for Duty Cycle value
char Volt_Txt[]     = "0.00V";
char RTC_ALARM_Txt[]= "0";                      //
char RPM_Txt[]      = "0000";               // text storage for RPM

char setup_time[]       = "00:00:00";
char setup_date[]       = "01/01/00";
char setup_alarm_time[] = "00:00:00"; 
char setup_heater_set_text[]   = "075F";


void putch (char c)
{   
    while (!TRMT);       
    TXREG = c;
}

void init_UART()
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 25);
    OSCCON = 0x70;
}



void Do_Init()                      // Initialize the ports 
{ 
    init_UART();                    // Initialize the uart
    Init_ADC();
    OSCCON=0x70;                    // Set oscillator to 8 MHz 
    
    ADCON1=0x0E; //ADCON1 is 0x0e because of the corresponding ports
    TRISA = 0x01; //Only an input on RA0
    TRISB = 0x03; //Outputs except for RB0 and RB1
    TRISC = 0x01; //Ouputs except for Rc1
    TRISD = 0x40; //One input on RD6
    TRISE = 0x00;
    PORTE = 0x00;

    HEATER = 0; //start with heater off
    RBPU=0;

    TMR3L = 0x00;                   
    T3CON = 0x03;
    I2C_Init(100000); 
    ALARMEN =0;

    DS1621_Init();
    Init_Interrupt();
    Turn_Off_Fan();
    heater_set_temp = 75;
}


void main() 
{
    Do_Init();                                                  // Initialization  
    Initialize_Screen();  
    DS3231_Turn_Off_Alarm();                                    
    DS3231_Read_Alarm_Time();                                   // Read alarm time for the first time
    color =0;
    tempSecond = 0xff;
    while (1)
    {
        DS3231_Read_Time();

        if(tempSecond != second)
        {
            tempSecond = second;
            rpm = get_RPM();
            volt = read_volt();
            DS1621_tempC = DS1621_Read_Temp();
            if ((DS1621_tempC & 0x80) == 0x80) DS1621_tempC = - (0x80 - DS1621_tempC & 0x7f);
            DS1621_tempF = (DS1621_tempC * 9 / 5) + 32;
            Set_RPM_RGB(rpm);
            Set_DC_RGB(duty_cycle);
            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year);
            printf (" Temp = %d C = %d F ", DS1621_tempC, DS1621_tempF);
            printf ("alarm = %d Heater = %d ", RTC_ALARM_NOT, HEATER);
            printf ("RPM = %d  dc = %d\r\n", rpm, duty_cycle);
            
            if (HEATER ==1) //If the heater is on turn the FAN led on, otherwise turn it off
            {
                FAN_LED =1;
            }
            else
            {
                FAN_LED =0;
            }
            Monitor_Heater(); 
            Test_Alarm();
            Update_Screen(); //update screen every second
        }
        
        if (check_for_button_input() == 1) //Code provided by professor to scan which button is being pressed
        {

            nec_ok = 0;
            switch (found)
            {
                case Ch_Minus:        
                    Do_Beep_Good();
                    Do_Setup_Time();
                    break;
                
                case Channel:           
                    Do_Beep_Good();
                    Do_Setup_Alarm_Time();
                    break;
                    
                case Ch_Plus:
                    Do_Beep_Good();
                    Do_Setup_Heater_Temp();            
                    break;
                    
                case Play_Pause:
                    Do_Beep_Good();
                    Toggle_Heater();
                     break;           
        
                default:
                    Do_Beep_Bad();
                    break;
            }
        }    


        if (INT1_flag == 1) //our button is on INT1, so we use the int1 software flag.
        {
            INT1_flag = 0; //clear the flag
            if (ALARMEN ==0) //Toggle the alarm enable whenever the button is pressed
            {
                ALARMEN =1;
            }
            else
            {
                ALARMEN = 0;
            }
            
            
        }
        
//        if (INT2_flag == 1)
//        {
//            INT2_flag = 0;
//        }
        
    }
}

void Test_Alarm()
{
     
    if (alarm_mode ==0&&ALARMEN ==1) // if the alarm is currently off but we toggle alarm mode button, turn on the alarm and change alarm_mode
    {
        DS3231_Turn_On_Alarm(); // turn on alarm
        alarm_mode =1;//Set alarm mode to on
    }    
    if (alarm_mode == 1 && ALARMEN ==0)// if alarm mode is on, but the enable is off we turn off alarm
    {
        DS3231_Turn_Off_Alarm(); //turn off the alarm
        alarm_mode = 0; //Turn alarm mode to zero
        Deactivate_Buzzer(); //turn the buzzer off
        PORTE = 0x00; //turn off the LED
        color = 0; //reset so that we start at 0 on the LED
    }
    if (alarm_mode ==1 && ALARMEN ==1) //if the enable is on and the mode is on, the alarm is fully on and we begin to check
    {
        printf("Alarmen = %d \r\n Alarmmode = %d\r\n RTCAlarm = %d\r\n",ALARMEN,alarm_mode,RTC_ALARM_NOT);
        if (RTC_ALARM_NOT == 0) //if the clock matches the alarm time we set off the alarm
        {
            Activate_Buzzer(); //Start buzzing
            MATCHED = 1;     //Turn matched to 1
            volt=read_volt(); //Get the new voltage
            Set_RGB_Color(color); //Cycle the color on the Alarm LED
            color++;//increment color by 1
            if (color>=8)color =0; // if color goes over 8 reset it to 0
            if (volt > 3.0) // if voltage greater than 3 break out of the alarm
            {
                MATCHED =0; //set matched to 0
                alarm_mode = 0; //set alarm mode to 0
                Deactivate_Buzzer(); //stop buzzing
                PORTE=0x00; //turn off alarm LED
            }

                
        }
            
    }
        
    }  
