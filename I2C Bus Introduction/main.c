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
#include "ST7735.h"
 //Pragmas below, include headers above
#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF
#pragma config CCP2MX = PORTBE

#define _XTAL_FREQ  8000000             // Set operation for 8 Mhz
#define Circle_Size     20              // Size of Circle for Light
#define Circle_X        60              // Location of Circle
#define Circle_Y        80              // Location of Circle
#define Text_X          52
#define Text_Y          77
#define TS_1            1               // Size of Normal Text
#define TS_2            2               // Size of Big Text


// colors
#define RD               ST7735_RED
#define BU               ST7735_BLUE
#define GR               ST7735_GREEN
#define MA               ST7735_MAGENTA
#define BK               ST7735_BLACK

#define kp               PORTEbits.RE1     

#define Circle_Size     20              // Size of Circle for Light
#define Circle_X        60              // Location of Circle
#define Circle_Y        80              // Location of Circle
#define Text_X          52
#define Text_Y          77
#define TS_1            1               // Size of Normal Text
#define TS_2            2               // Size of Big Text

#define RD               ST7735_RED
#define BU               ST7735_BLUE
#define GR               ST7735_GREEN
#define MA               ST7735_MAGENTA
#define BK               ST7735_BLACK

#define D1R 0x04 // Defining hex values to turn on pertinent LED with color.
#define D1B 0x10
#define D1W 0x1c
#define D1M 0x14
#define D2R 0x10 
#define D2B 0x20
#define D2W 0x70
#define D2M 0x30
#define D3R 0x01
#define D3G 0x02
#define D3W 0x07
#define D2G 0x40
#define D3M 0x05

 //initialize variables.
char tempSecond = 0xff; 
char second = 0x00;
char minute = 0x00;
char hour = 0x00;
char dow = 0x00;
char day = 0x00;
char month = 0x00;
char year = 0x00;
char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
char alarm_second, alarm_minute, alarm_hour, alarm_date;
char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;
short nec_ok;


char array1[21]={0xa2,0x62,0xe2,0x22,0x02,0xc2,0xe0,0xa8,0x90,0x68,0x98,0xb0,0x30,0x18,0x7a,0x10,0x38,0x5a,0x42,0x4a,0x52}; //array of hex keys for IR Reciever
char txt1[21][4] ={"CH-\0", "CH\0", "CH+\0","PRV\0","NXT\0","PSE\0","VL+\0","VL-\0","EQ \0", " 0 \0", "100\0", "200\0", " 1 \0"," 2 \0", " 3 \0"," 4 \0"," 5 \0", " 6 \0", " 7 \0", " 8 \0", " 9 \0"}; //Array for the LCD screen
int color[21]={RD,RD,RD,BU,BU,GR,MA,MA,MA,BK,BK,BK,BK,BK,BK,BK,BK,BK,BK,BK,BK}; //Array for colors. 
char d1[21] = {D1R,D1R,D1R,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //Diode 1 Array
char d2[21] = {0,0,0,D2B,D2B,D2G,D2M,D2M,D2M,0,0,0,0,0,0,0,0,0,0,0,0}; //Diode 2 array.
char d3[21] = {0,0,0,0,0,0,0,0,0,D3W,D3W,D3W,D3W,D3W,D3W,D3W,D3W,D3W,D3W,D3W,D3W}; //Diode 3 outputs 

char TempSecond;

char buffer[31];                        // general buffer for display purpose
char *nbr;                              // general pointer used for buffer
char *txt;

char Nec_code1;
char found;
//Prototype area
void Deactivate_Buzzer();
void Activate_Buzzer();
void Initialize_Screen();
void Wait_One_Sec();
void Activate_LED();
void Deactivate_LED();
void Wait_Half_Sec();
void Do_Init();
void main() 
{ 
    Do_Init();                                                  // Initialization  
    Initialize_Screen();
    while(1)
    {
         DS3231_Read_Time(); //Read time at first
         if (tempSecond != second) //Wait 1 second
         {
             tempSecond = second;
             signed char tempC = DS1621_Read_Temp();
             signed char tempF = (tempC * 9 / 5) + 32; //Calculate temperature in F and C according to the information given my DS1621

             printf ("%02x:%02x:%02x %02x/%02x/%02x  |",hour,minute,second,month,day,year); //output onto teraterm all informtion
             printf (" Temperature = %d degrees C = %d degrees F\r\n", tempC, tempF);
         }
        if (nec_ok == 1)					// This is for the final section
        {
            nec_ok = 0;


            INTCONbits.INT0IE = 1;          // Enable external interrupt
            INTCON2bits.INTEDG0 = 0;        // Edge programming for INT0 falling edge
            for (int i = 0; i <21; i++) //search for found
            {
                if (array1[i] == Nec_code1) //set found if the hex value is equivalnet
                {
                    found = i; //setting variable

                    
                }
            }

            if (Nec_code1 == 0x90)
            {
                DS3231_Setup_Time(); //If the key is the EQ button setup the time
            }
            printf("\n Found key is: %d\r\n", found); //print out the found key onto tera term each time a button is pressed
            
           // printf("\n FOUND IS: %d \r\n",found);
            PORTA = d1[found]; //Outputing to diode 1
            PORTB = d2[found]|PORTBbits.RB0|PORTBbits.RB3; //outputting to diode 2
            PORTE = d3[found]; //Output to diode 3
            if (found != 0xff)  ///output to the LCD screen found
            {
                fillCircle(Circle_X, Circle_Y, Circle_Size, color[found]); 
                drawCircle(Circle_X, Circle_Y, Circle_Size, ST7735_WHITE);  
                drawtext(Text_X, Text_Y, txt1[found], ST7735_WHITE, ST7735_BLACK,TS_1); 
            }
            Activate_LED(); //turn on LED
             Activate_Buzzer(); //Turn on buzzer
            Wait_Half_Sec(); //wait half a second
            Deactivate_LED(); // Turn off led
            Deactivate_Buzzer(); //turn buzzer off 

    
        }
         //infinite loop
    }
    
    
}

void putch (char c) //code given to us for tera term
{   
    while (!TRMT);       
    TXREG = c;
}

void init_UART() //Code given to us for tera term
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 25);
    OSCCON = 0x70;
}


void Wait_One_Sec() //Wait one second.
{
    for (int k = 0; k< 17000; k++);
}
void Activate_LED() //Turn the LED on
{
    PORTDbits.RD7 = 1;
}
void Deactivate_LED() //Turn the LED Off
{
    PORTDbits.RD7 = 0;
}


void Do_Init()                    // Initialize the ports 
{ 
    init_UART();                    // Initialize the uart
    init_INTERRUPT();
    OSCCON=0x70;                    // Set oscillator to 8 MHz 
    
    ADCON1= 0x0f;		    // Fill out values
    TRISA = 0x00; //POrt a is all outputs
    TRISB = 0x01; // POrt b has 1 input
    TRISC = 0x00;                    //Trisc is all output
    TRISD = 0x00; // Tris d is all outputs
    TRISE = 0x00; // TRISE is all outputs
    RBPU=0; //Code provided to help initialzie the program
    I2C_Init(100000); 
    nec_ok = 0;
    DS1621_Init(); //initiaize the device

}





void Activate_Buzzer() //TUrn on the buzzer as used in previous programs
{
PR2 = 0b11111001 ;
T2CON = 0b00000101 ;
CCPR2L = 0b01001010 ;
CCP2CON = 0b00111100 ;

}

void Deactivate_Buzzer()//Turn off the buzzer as used in previous programs
{
    CCP2CON = 0;
    PORTBbits.RB3 = 0;
}

void Initialize_Screen() //Code provided by the professor for this lab
{
    LCD_Reset();
    TFT_GreenTab_Initialize();
    fillScreen(ST7735_BLACK);
  
    /* TOP HEADER FIELD */
    txt = buffer;
    strcpy(txt, "ECE3301L Spring 22-S4");  
    drawtext(2, 2, txt, ST7735_WHITE, ST7735_BLACK, TS_1);

    strcpy(txt, "LAB 11 ");  
    drawtext(50, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
}
void Wait_Half_Sec() //Waiting half a second as used in previous labs.
{
 T0CON = 0x03; // Timer 0, 16-bit mode, prescaler 1:8
 TMR0L = 0xDB; // set the lower byte of TMR
 TMR0H = 0x0B; // set the upper byte of TMR
 INTCONbits.TMR0IF = 0; // clear the Timer 0 flag
 T0CONbits.TMR0ON = 1; // Turn on the Timer 0
 while (INTCONbits.TMR0IF == 0); // wait for the Timer Flag to be 1 for done
 T0CONbits.TMR0ON = 0; // turn off the Timer 0
 
}
