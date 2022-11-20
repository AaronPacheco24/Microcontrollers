#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h> //#include <usart.h> 
#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF  // Lines 1-8 standard for programs
#define delay 5
#define D1_RED PORTBbits.RB3 //D1Red is connected to Rb3
#define D1_GREEN PORTBbits.RB4 //D1 Green is connected to Rb4
#define D1_BLUE PORTBbits.RB5 //D1 Blue is connected to rb5
#define D2_RED PORTBbits.RB0 //D2_red is connected to rb0
#define D2_GREEN PORTBbits.RB1 // D2_Green is connected to Rb1 
#define D2_BLUE PORTBbits.RB2 //D2 Blue is connected to RB2
#define D3_RED PORTAbits.RA4 // D3 Red is connected to ra4
#define D3_GREEN PORTAbits.RA5 //D3 Green is connected to Ra5
// ALL FUNCTIONS AND GLOBA VARIABLES DECLARED BELOW
char array[10] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x18}; //array for the 7 segment display
char d2array[8] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07};
void Init_ADC();
void WAIT_1_SEC();
void Display_Upper_Digit(char digit);        
void Display_Lower_Digit(char digit);
void Select_ADC_Channel(char channel); //
 //Global array for the numbers of 7segdisplay
void Init_ADC(void); //Initialize ADC
unsigned int Get_Full_ADC(void); //get the full analog to digital converters proper value
float Read_Ch_Volt(char); //for reading of voltage
void init_UART(); //for terraterm implementation
void putch (char c); // For teraterm implementation
void init_TRIS(); // initialize all tris/
void SET_D1_OFF(); //Lines 30-42 set the value for each bit to the relevant one for  diodes 1,2, and 3 so that they have the right color.
void SET_D2_OFF();
void SET_D1_RED();
void SET_D1_GREEN();
void SET_D1_BLUE();
void SET_D1_WHITE();
void SET_D2_RED();
void SET_D2_GREEN();
void SET_D2_YELLOW();
void SET_D2_BLUE();
void SET_D2_PURPLE();
void SET_D2_CYAN();
void SET_D2_WHITE();
void SET_D3_RED();
void SET_D3_GREEN();
void SET_D3_YELLOW();
void diode_1(int z);
void diode_2(int t);
void diode_3();
//END OF PROTOTYPE AREA

void main()
{
    
    init_UART(); 
    Init_ADC(); // initialize analog to digital    
    init_TRIS();//initialize the tris
    while (1)
    {

        Select_ADC_Channel (0); //Begin with channel 4 
        int num_step = Get_Full_ADC();  //The numsp is needed to calculate the voltage. Tuhs we need the full ADC information
        float voltage_mv = num_step* 4.0; //Voltage  = step * 4 as discussed in lecture
        float temperature_C = (1035.0-voltage_mv)/5.50; //This is the calculation provided by texas instrments. It is in celcius so we work with celcius
        float temperature_F = (1.80*temperature_C) + 32.0; // we then move celcius to farenheit.
        int tempF = (int) temperature_F; // Convert farehnheit to an interger
        printf("tempf = %d\r\n",tempF);
        printf("TemperatureFloat = %f\r\n",temperature_F);
        char U = tempF/10; // Divide by 10 and place into U for upper digit
        char L = tempF%10; //modulus 10 for one's place for the lower digit
        Display_Upper_Digit(U); //Send the upper digit to the display function
        Display_Lower_Digit(L); //Send the lower digit to the display function
        diode_1(tempF); //Diode one goes with the full temperature
        diode_2(U); // Diode 2 is called with only the upper digit
        diode_3(); //diode 3 is called with no information
        printf("Temperature = %d \r\n",tempF); // TERATERM
        Select_ADC_Channel (1); //Change to channel 1 for the voltage calculation
        num_step = Get_Full_ADC();  //The numsp is needed to calculate the voltage. Tuhs we need the full ADC information
        voltage_mv = num_step * 4.0; //Voltage(in millivolts) = step * 4 as discussed in lecture
        float voltage = (voltage_mv / 1000); // Volts = millivolts / 1000
        printf("Voltage = %f \r\n", voltage); //print voltage
        WAIT_1_SEC(); // wait 1 second before looping
    }
}
void Init_ADC() //initialize ADCON 1 and ADCON2.
{ 
    ADCON1 = 0x1B; //We have an1-an3 as analogRest are digital Which results in 1B
    ADCON2 = 0xA9; 
}
void WAIT_1_SEC() //Wait 1 second such that we can see what happens in the loop
{
    for (int j = 0;j<17000;j++);
}
void Display_Lower_Digit(char digit) //This displays the lower digit on the 7-segment display.
{
    PORTD = array[digit];
//    PORTD = array1[1];
}
void Display_Upper_Digit(char digit)
{
//    PORTC = array1[2];
    PORTC = array[digit] &0x3f; //mask some of the data
    if( array[digit] == 0x40 || array[digit] == 0x79 || array[digit] == 0x78) //these are instances where segment G is off
    {
        PORTEbits.RE2 = 1;
    }
    else
    {
        PORTEbits.RE2 = 0; //otherwise turn segment G back on
        
    }
}
void Select_ADC_Channel( char channel) //This selects the ADC channel to be used for ADCON0.
{
    ADCON0 = (channel *4) +1;//Shift left by 2 to move it into channel position of ADCON and add 1 for ADON.
}
unsigned int Get_Full_ADC(void) //This gets the Full ADC for ADCOn
{
int result;
 ADCON0bits.GO=1; // Start Conversion
 while(ADCON0bits.DONE==1); // Wait for conversion to be completed (DONE=0)
 result = (ADRESH * 0x100) + ADRESL; // Combine result of upper byte and lower byte into
 return result; // return the most significant 8- bits of the result.
}
float Read_Ch_Volt(char ch_num) //Read voltage. Not used in this program
{
 ADCON0 = ch_num * 0x4 + 1;
 int ADC_Result = Get_Full_ADC();
 float Volt = ADC_Result / 1024.0 * 5.0;
 return (Volt);
} 
//Init_UART and PUTCH are responsible for reading and printing the data onto the tera term software
void init_UART()
{
 OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
USART_BRGH_HIGH, 25);
 OSCCON = 0x60;
}
void putch (char c)
{
    while (!TRMT);
    TXREG = c;
}
//the following functions force the pertinent Diodes to the respective colors by changing the relevant bits to 1s and 0s.
void SET_D1_OFF()
{
    D1_RED = 0;
    D1_GREEN = 0;
    D1_BLUE = 0;
}
void SET_D2_OFF()
{
    D2_RED = 0;
    D2_GREEN =0;
    D2_BLUE = 0;
}
void SET_D1_RED()
{
    D1_RED = 1;
    D1_GREEN = 0;
    D1_BLUE = 0;
}
void SET_D1_GREEN()
{
    D1_RED = 0;
    D1_GREEN = 1;
    D1_BLUE = 0;
}
void SET_D1_BLUE()
{
    D1_RED = 0;
    D1_GREEN = 0;
    D1_BLUE = 1;  
}
void SET_D1_WHITE()
{
    D1_RED = 1;
    D1_GREEN = 1;
    D1_BLUE = 1;
}
void SET_D2_RED()
{
    D2_RED = 1;
    D2_GREEN = 0;
    D2_BLUE = 0;
}
void SET_D2_GREEN()
{
    D2_RED = 0;
    D2_GREEN = 1;
    D2_BLUE = 0;
}
void SET_D2_YELLOW()
{
    D2_RED = 1;
    D2_GREEN = 1;
    D2_BLUE = 0;
}
void SET_D2_BLUE()
{
    D2_RED = 0;
    D2_GREEN = 0;
    D2_BLUE = 1;
}
void SET_D2_PURPLE()
{
    D2_RED = 1;
    D2_GREEN = 0;
    D2_BLUE = 1;
}
void SET_D2_CYAN()
{
    D2_RED = 0;
    D2_GREEN = 1;
    D2_BLUE = 1;
}
void SET_D2_WHITE()
{
    D2_RED = 1;
    D2_GREEN = 1;
    D2_BLUE = 1;
}
void SET_D3_RED()
{
    D3_RED = 1;
    D3_GREEN = 0;
}
void SET_D3_GREEN()
{
    D3_RED = 0;
    D3_GREEN = 1;
}
void SET_D3_YELLOW()
{
    D3_RED = 1;
    D3_GREEN = 1;
}
void diode_1(int z)
{
    if (z < 45)
    {
        SET_D1_OFF(); //if temp less than 45 set off
    }
    else
    {
        if (z >45 && z <56)
        {
            SET_D1_RED(); //if temp between 45 and 56 set to red
        }
        else
        {
            if ( z>55 && z<66)
            {
                SET_D1_GREEN(); //if temp between 55 and 66 set green
            }
            else
            {
                if ( z> 65 && z < 76) //if temp between 65 and 76 set to blue
                {
                    SET_D1_BLUE();
                }
                else
                {
                    SET_D1_WHITE(); //if temp above 76 set to white
                }
            }
        }

    }
     
}
void diode_2(int t)
{
    if (t < 1) //if less than 1 (0) set to off)
    {
        SET_D2_OFF();
    }
    if (t > 7) SET_D2_WHITE(); // if greater than 7 keep white
    switch(t){
        case(0):
            SET_D2_OFF(); //set to off
            break;
        case(1):
            SET_D2_RED(); //set to white if 10s = 1
            break;
        case(2):
            SET_D2_GREEN(); //set to green if 10s = 2
            break;
        case(3):
            SET_D2_YELLOW(); //set to yellow if temperature is in the 30s
            break;
        case(4):
            SET_D2_BLUE(); //set d2 to blue if temperature is in the 40s
            break;
        case(5):
            SET_D2_PURPLE(); //set d2 to purple if temperature is in the 50s
            break;
        case(6):
            SET_D2_CYAN(); //set d2 to cyan if temperature is in the 60s
            break;
        case(7):
            SET_D2_WHITE(); // set d2 to white if temperature is in the 70s
            break;
    }
}
void diode_3()
{
    Select_ADC_Channel (1);
    int num_step = Get_Full_ADC();  //The numsetp is needed to calculate the voltage. Tuhs we need the full ADC information
    float voltage_mv = num_step*4.0; //Voltage  = step * 4 as discussed in lecture
    float volts = voltage_mv / 1000; //calculate voltage in volts
    if (volts < 2.5)
    {
        SET_D3_RED(); //if onver 2.5 set to red
    }
    else
    {
        if (volts >= 2.5  && volts <3.5) // if voltage between 2.5 and 3.5  set to gree
        {
            SET_D3_GREEN();
        }
        else
        {
            if (volts > 3.4){
            SET_D3_YELLOW();} //if volts over 3.5 set d3 to yellow   
        }
    }
}
void init_TRIS()
{
    TRISA = 0x0F; //TRISA RA0-RA3 are inputs 
    TRISB = 0x00; //output
    TRISC = 0x00; //output
    TRISD = 0x00; //output
    TRISE = 0x00; // output
    
}

