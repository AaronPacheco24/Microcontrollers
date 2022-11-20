#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>
#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF // Lines 1-8 standard for programs
#pragma config CCP2MX = PORTBE
#define _XTAL_FREQ 8000000         // Set operation for 8 Mhz
#define TMR_CLOCK _XTAL_FREQ/4    // Timer Clock 2 Mhz
#define TFT_DC PORTCbits.RC1 
#define TFT_CS PORTCbits.RC2
#define TFT_RST PORTCbits.RC0
#define SEC_LED PORTDbits.RD7
#define NS_RED PORTAbits.RA1
#define NS_GREEN PORTAbits.RA2
#define NSLT_RED PORTAbits.RA3
#define NSLT_GREEN PORTAbits.RA4 
//#define NSPED_SW PORTAbits.RA5
//#define EWPED_SW PORTBbits.RB0
#define NSLT_SW PORTAbits.RA5
#define EWLT_SW PORTCbits.RC4
#define EW_RED PORTBbits.RB4
#define EW_GREEN PORTBbits.RB5
#define EWLT_RED PORTEbits.RE0
#define EWLT_GREEN PORTEbits.RE2
#define OFF 0
#define RED 1 
#define GREEN 2
#define YELLOW 3
#define SEG_G PORTEbits.RE1
#define MODE_LED PORTBbits.RB6
#define Circle_Size 7
#define Circle_Offset 15
#define TS_1 1
#define TS_2 2
#define Count_Offset 10
#define XTXT 30              // X location of Title Text 
#define XRED 40              // X location of Red Circle
#define XYEL 60              // X location of Yellow Circle
#define XGRN 80              // X location of Green Circle
#define XCNT 100             // X location of Sec Count
#define EW 0
#define EWLT 1
#define NS 2
#define NSLT 3
#define Color_Off 0
#define Color_Red 1
#define Color_Green 2
#define Color_Yellow 3
#define EW_Txt_Y 20
#define EW_Circ_Y EW_Txt_Y + Circle_Offset
#define EW_Count_Y EW_Txt_Y + Count_Offset
#define EW_Color ST7735_CYAN
#define EWLT_Txt_Y 50
#define EWLT_Cir_Y EWLT_Txt_Y + Circle_Offset
#define EWLT_Count_Y EWLT_Txt_Y + Count_Offset
#define EWLT_Color ST7735_WHITE
#define NS_Txt_Y 80
#define NS_Cir_Y  NS_Txt_Y + Circle_Offset
#define NS_Count_Y NS_Txt_Y + Count_Offset
#define NS_Color ST7735_BLUE 
#define NSLT_Txt_Y 110
#define NSLT_Cir_Y NSLT_Txt_Y + Circle_Offset
#define NSLT_Count_Y NSLT_Txt_Y + Count_Offset
#define NSLT_Color ST7735_MAGENTA
#define PED_EW_Count_Y 30
#define PED_NS_Count_Y 90
#define PED_Count_X 2
#define Switch_Txt_Y 140
#include "ST7735_TFT.c"
char buffer[31];                        // general buffer for display purpose
char *nbr;                              // general pointer used for buffer
char *txt;
char EW_Count[]     = "00";             // text storage for EW Count
char EWLT_Count[]   = "00";             // text storage for EW Left Turn Count
char NS_Count[]     = "00";             // text storage for NS Count
char NSLT_Count[]   = "00";             // text storage for NS Left Turn Count
char PED_EW_Count[] = "00";             // text storage for EW Pedestrian Count
char PED_NS_Count[] = "00";             // text storage for NS Pedestrian Count
char SW_EWPED_Txt[] = "0";              // text storage for EW Pedestrian Switch
char SW_EWLT_Txt[]  = "0";              // text storage for EW Left Turn Switch
char SW_NSPED_Txt[] = "0";              // text storage for NS Pedestrian Switch
char SW_NSLT_Txt[]  = "0";              // text storage for NS Left Turn Switch
char SW_MODE_Txt[]  = "D";              // text storage for Mode Light Sensor
char Act_Mode_Txt[]  = "D";             // text storage for Actual Mode
char FlashingS_Txt[] = "0";             // text storage for Emergency Status
char FlashingR_Txt[] = "0";             // text storage for Flashing Request
char FlashingA_Txt[] = "*";             //tEXT STOREAGE FOR FLASHING EVERY SECOND
char dir;
char Count;                             // RAM variable for Second Count
char PED_Count;                         // RAM variable for Second Pedestrian Count
char SW_EWPED;                          // RAM variable for EW Pedestrian Switch
char SW_EWLT;                           // RAM variable for EW Left Turn Switch
char SW_NSPED;                          // RAM variable for NS Pedestrian Switch
char SW_NSLT;                           // RAM variable for NS Left Turn Switch
char FLASHING;
char FLASHING_REQUEST;
char SW_MODE;                           // RAM variable for Mode Light Sensor
int MODE;
char direction;
float volt;
char array[10] = {0x7f,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x18}; //prototype area
void display_upper_digit(char digit);
void display_lower_digit(char digit);
void init_ADC();
void Wait_One_Second();
void Wait_Half_Second();
void Wait_N_Seconds(char seconds);
void Set_NS(char color);
void Set_NSLT(char color);
void Set_EW(char color);
void Set_EWLT(char color);
void PED_Control(char direction, char num_sec);
void putch (char c);
void init_UART();
void init_TRIS();
void Wait_One_Second_With_Beep();
void Activate_Buzzer();
void Deactivate_Buzzer();
void Day_Mode();
void Night_Mode();
unsigned int Get_Full_ADC(void);
float Read_Ch_Volt(char); 
void Initialize_Screen();
void update_LCD_color(char direction,char color);
void update_LCD_count(char direction, char count);
void update_LCD_PED_Count(char direction, char count);
void update_LCD_misc();
void Do_Init();
void INT0_ISR();
void INT1_ISR();
void INT2_ISR();
//int INT0_flag, INT1_flag, INT2_flag = 0;
char NS_PED_SW;
char EW_PED_SW;
void Do_Flashing();
char FLASHING_REQUEST;
char FLASHING;
void interrupt high_priority chkisr();
void main()
{
    Do_Init();
    float voltage_MV; //DECLARE VARIBALES
    float voltage;//DECLARE VARIABLS
    while (1) //INFINITE LOOP   
    {
        voltage_MV = Read_Ch_Volt(0); // CALCULATE VOLATE
        voltage = (voltage_MV); 
        printf("Voltage = %f \r\n", voltage); //print voltae
        NS_PED_SW = 0;
        EW_PED_SW = 0;
        if(voltage <2.5)
        {
 
            MODE_LED = 1; //iF VOLTAGE IS LOW, IT IS DAY TIME
            MODE = 1;
            Act_Mode_Txt[0] = 'D'; // WRITE DAY ON SCREEN
            drawtext(35,10, Act_Mode_Txt, ST7735_WHITE, ST7735_BLACK, TS_1); //DRAW ON SCREEN
            Day_Mode(); //SEND TO DAY FUNCTION
        }
        else
        {
            //NS_PED_SW = 0;
           // EW_PED_SW = 0;
            MODE = 0;
            MODE_LED = 0; //ELSE CHANGE TO NIGHT MODE
            Act_Mode_Txt[0] = 'N'; //CHANGE TO N
            drawtext(35,10, Act_Mode_Txt, ST7735_WHITE, ST7735_BLACK, TS_1); //WRITE N ON SCREEN
            Night_Mode(); //GO TO NIGHT MODE
        }
        //\Wait_One_Second();
        if (FLASHING_REQUEST ==1)
        {
            FLASHING_REQUEST = 0;
            Do_Flashing();
        }
    }
}
void Wait_One_Second()
{
 drawtext(120, 10, FlashingA_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);  //EVERY SECOND FLASH ASTERIST ON

 update_LCD_misc();
 SEC_LED = 0; // First, turn off the SEC LED
 
 Wait_Half_Second(); // Wait for half second (or 500 msec)
 SEC_LED = 1; // then turn on the SEC LED
   drawtext(120, 10, FlashingA_Txt, ST7735_BLACK, ST7735_BLACK, TS_1);  //FLASH THE ASTERISK OFF

 Wait_Half_Second(); // Wait for half second (or 500 msec)

}
void Wait_Half_Second()
{
 T0CON = 0x03; // Timer 0, 16-bit mode, prescaler 1:8
 TMR0L = 0xDB; // set the lower byte of TMR
 TMR0H = 0x0B; // set the upper byte of TMR
 INTCONbits.TMR0IF = 0; // clear the Timer 0 flag
 T0CONbits.TMR0ON = 1; // Turn on the Timer 0
 while (INTCONbits.TMR0IF == 0); // wait for the Timer Flag to be 1 for done
 T0CONbits.TMR0ON = 0; // turn off the Timer 0
 
} 

void Wait_N_Seconds (char seconds)
{
char I;
 for (I = seconds; I> 0; I--)
 {
     Wait_One_Second(); // FOR EACH I WAIT ONE SECOND
     update_LCD_count(direction, I);  //EACH ITERATION UPDATE THE SCREEN
 }
update_LCD_count(direction, 1); // UPDATE THE LCD COUNTER
Wait_One_Second(); //WAIT AGAIN
update_LCD_count(direction, 0); // TO MAKE SURE IT HITS ZERO GO ONCE MORE
} 
void Set_NS(char color)
{
    direction = NS; //
    update_LCD_color(direction,color); // UPDATE THE LCD WITH NS
 switch (color)
 {
     case OFF: NS_RED =0;NS_GREEN=0;break; // Turns off the NS LED
     case RED: NS_RED =1;NS_GREEN=0;break; // Sets NS LED RED
     case GREEN: NS_RED =0;NS_GREEN=1;break; // sets NS LED GREEN
     case YELLOW: NS_RED =1;NS_GREEN=1;break; // sets NS LED YELLOW
 } 
}
void Set_NSLT(char color)
{    
    direction = NSLT;
    update_LCD_color(direction,color); // UPDATE THE LCD WITH NSLT
 switch (color)
 {
     case OFF: NSLT_RED =0;NSLT_GREEN=0;break; // Turns off the NS LED
     case RED: NSLT_RED =1;NSLT_GREEN=0;break; // Sets NS LED RED
     case GREEN: NSLT_RED =0;NSLT_GREEN=1;break; // sets NS LED GREEN
     case YELLOW: NSLT_RED =1;NSLT_GREEN=1;break; // sets NS LED YELLOW
 } 
}
void Set_EW(char color)
{
    direction = EW;
    update_LCD_color(direction,color); //UPDATE THE LCD WITH EW
 switch (color)
 {
     case OFF: EW_RED =0;EW_GREEN=0;break; // Turns off the NS LED
     case RED: EW_RED =1;EW_GREEN=0;break; // Sets NS LED RED
     case GREEN: EW_RED =0;EW_GREEN=1;break; // sets NS LED GREEN
     case YELLOW: EW_RED =1;EW_GREEN=1;break; // sets NS LED YELLOW
 } 
 
}
void Set_EWLT(char color)
{
    direction = EWLT;
    update_LCD_color(direction,color); //UPATE THE LCD WITH EWLT
 switch (color)
 {
     case OFF: EWLT_RED =0;EWLT_GREEN=0;break; // Turns off the NS LED
     case RED: EWLT_RED =1;EWLT_GREEN=0;break; // Sets NS LED RED
     case GREEN: EWLT_RED =0;EWLT_GREEN=1;break; // sets NS LED GREEN
     case YELLOW: EWLT_RED =1;EWLT_GREEN=1;break; // sets NS LED YELLOW
 } 
}
void PED_Control(char direction, char num_sec)
{
    //display_lower_digit(0x00);
    //display_upper_digit(0x00);
    for (char i = num_sec;i>0;i--) //LOOP
    {
        //if (direction ==0)
        //{
         //   void update_PED_Count(char direction, char count);
        //}
        //else
        //{
         //   void update_PED_Count(char direction=, char count);
        //}
        update_LCD_PED_Count(direction, i); //FOR EACH ITERATION UPDATE LCD SCREEN ON PEDESTRIAN
        Wait_One_Second_With_Beep(); //BEEP AND GO BACK
    }
    update_LCD_PED_Count(direction, 0); //ONE MORE ITERATION TO ENSURE IT HITS 0
    //display_upper_digit(0x00);
    //display_lower_digit(0x00);
    Wait_One_Second_With_Beep(); //ENSURES THE LAST BEEP
    
 }
void putch (char c)
{
    while (!TRMT);
    TXREG = c;
}
void init_UART() 
{
 OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
USART_BRGH_HIGH, 25);
 OSCCON = 0x70;
}
void init_ADC() //INITIALIZE ANALOG TO DIGITAL
{
    ADCON0 = 0x01;
    ADCON1 = 0x0E;
    ADCON2 = 0xA9;
}
void init_TRIS() //INITIALIZE TRIS
{
    int result;
    TRISA = 0x21;
    TRISB = 0x07;
    TRISC = 0x10;
    TRISD = 0x00;
    TRISE = 0x00;
    
}
void Wait_One_Second_With_Beep()
{
 SEC_LED = 1; // First, turn on the SEC LED
Activate_Buzzer(); // Activate the buzzer
 Wait_Half_Second(); // Wait for half second (or 500 msec)
 SEC_LED = 0; // then turn off the SEC LED
 Deactivate_Buzzer (); // Deactivate the buzzer
 Wait_Half_Second(); // Wait for half second (or 500 msec)
}
void Activate_Buzzer() //CODE GIVEN TO US BY PROFESSOR
{
PR2 = 0b11111001 ;
T2CON = 0b00000101 ;
CCPR2L = 0b01001010 ;
CCP2CON = 0b00111100 ;
}
void Deactivate_Buzzer() ///CODE GIVEN TO US BY PROFESSOR
{
CCP2CON = 0x0;
 PORTBbits.RB3 = 0;
}
void display_upper_digit(char digit) // NOT USED IN THIS CODE
{
    PORTC = array[digit] & 0x3f;
    if ((array[digit] & 0x40) == 0x40) 
    {
        SEG_G = 1;
    }
    else
    { 
        SEG_G = 0;
    }
}
void display_lower_digit(char digit) //NOT USED IN THIS CODE
{
    PORTD = ((PORTD & 0x80) | array[digit]);
}
void Day_Mode() //DAY MODE AS REQUESTED
{
    Wait_Half_Second();
    Set_NSLT(RED); //step 1
    Set_EW(RED);
    Set_EWLT(RED);
    Set_NS(GREEN); 
    if(NS_PED_SW == 1) //step1
    {
        PED_Control(2,7);
        NS_PED_SW = 0;
    }
    Wait_N_Seconds(9);//step 2
    Set_NS(YELLOW);
    Wait_N_Seconds(3); //step3 
    Set_NS(RED);//step4
    if (EWLT_SW == 1) //step5
    {
        Set_EWLT(GREEN);//step6
        Wait_N_Seconds(7);
        Set_EWLT(YELLOW);//step7]
        Wait_N_Seconds(3);
        Set_EWLT(RED);//step8
    }
    Set_EW(GREEN); //step9  
    if (EW_PED_SW == 1)
    {
        PED_Control(0,9);//step9a
        EW_PED_SW = 0;
    }
    Wait_N_Seconds(8);//step10
    Set_EW(YELLOW);//step11
    Wait_N_Seconds(3);
    Set_EW(RED);//step12
    if(NSLT_SW==1) //step13
    {
        Set_NSLT(GREEN);//step14
        Wait_N_Seconds(7);
        Set_NSLT(YELLOW);//step15
        Wait_N_Seconds(3);
        Set_NSLT(RED);//step16
    }//done
}
void Night_Mode() // night mode steps as requested
{
    Set_NSLT(RED);//step1
    Set_EW(RED);
    Set_EWLT(RED);
    Set_NS(GREEN);//step1
    Wait_N_Seconds(9);//step2]
    Set_NS(YELLOW);//step3
    Wait_N_Seconds(3);
    Set_NS(RED);//step4
    if(EWLT_SW == 1)//step4
    { 
        Set_EWLT(GREEN);//step6
        Wait_N_Seconds(7);
        Set_EWLT(YELLOW);//step7
        Wait_N_Seconds(3);
        Set_EWLT(RED);//step8
    }
    Set_EW(GREEN);//step9
    Wait_N_Seconds(9);
    Set_EW(YELLOW);//step10
    Wait_N_Seconds(3);
    Set_EW(RED);//step11
    if(NSLT_SW == 1)//step12
    {
        Set_NSLT(GREEN);//step13
        Wait_N_Seconds(7);
        Set_NSLT(YELLOW);//step14
        Wait_N_Seconds(3);
        Set_NSLT(RED);//step15
    }//done
}
unsigned int Get_Full_ADC(void)
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
 float Volts = ADC_Result / 1000.0 * 5.0;
 return (Volts);
} 
void Initialize_Screen() //code provided by professor
{
  LCD_Reset();
  TFT_GreenTab_Initialize();
  fillScreen(ST7735_BLACK);
  
  /* TOP HEADER FIELD */
  txt = buffer;
  strcpy(txt, "ECE3301L Spring 22-S4");  
  drawtext(2, 2, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  
  /* MODE FIELD */
  strcpy(txt, "Mode:");
  drawtext(2, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(35,10, Act_Mode_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);

  /* FLASHING REQUEST FIELD */
  strcpy(txt, "FR:");
  drawtext(50, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(70, 10, FlashingR_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  
  /* FLASHING STATUS FIELD */
  strcpy(txt, "FS:");
  drawtext(80, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(100, 10, FlashingS_Txt, ST7735_WHITE, ST7735_BLACK, TS_1); 
  
  /* SECOND UPDATE FIELD */
  //strcpy(txt, "*");
  //drawtext(120, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  //drawtext(120,10, FlashingA_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  
/* EAST/WEST UPDATE FIELD */
  strcpy(txt, "EAST/WEST");
  drawtext  (XTXT, EW_Txt_Y, txt, EW_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, EW_Circ_Y-8, 60, 18, EW_Color);
  drawCircle(XRED, EW_Circ_Y, Circle_Size, ST7735_RED);
  drawCircle(XYEL, EW_Circ_Y, Circle_Size, ST7735_YELLOW);
  fillCircle(XGRN, EW_Circ_Y, Circle_Size, ST7735_GREEN);
  drawtext(XCNT, EW_Count_Y, EW_Count, EW_Color, ST7735_BLACK, TS_2);    


  /* EAST/WEST LEFT TURN UPDATE FIELD */
  strcpy(txt, "E/W LT");
  drawtext  (XTXT, EWLT_Txt_Y, txt, EWLT_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, EWLT_Cir_Y-8, 60, 18, EWLT_Color);  
  fillCircle(XRED, EWLT_Cir_Y, Circle_Size, ST7735_RED);
  drawCircle(XYEL, EWLT_Cir_Y, Circle_Size, ST7735_YELLOW);
  drawCircle(XGRN, EWLT_Cir_Y, Circle_Size, ST7735_GREEN);   
  drawtext  (XCNT, EWLT_Count_Y, EWLT_Count, EWLT_Color, ST7735_BLACK, TS_2);

  /* NORTH/SOUTH UPDATE FIELD */
  strcpy(txt, "NORTH/SOUTH");
  drawtext  (XTXT, NS_Txt_Y  , txt, NS_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, NS_Cir_Y-8, 60, 18, NS_Color);
  fillCircle(XRED, NS_Cir_Y  , Circle_Size, ST7735_RED);
  drawCircle(XYEL, NS_Cir_Y  , Circle_Size, ST7735_YELLOW);
  drawCircle(XGRN, NS_Cir_Y  , Circle_Size, ST7735_GREEN);
  drawtext  (XCNT, NS_Count_Y, NS_Count, NS_Color, ST7735_BLACK, TS_2);
    
  /* NORTH/SOUTH LEFT TURN UPDATE FIELD */
  strcpy(txt, "N/S LT");
  drawtext  (XTXT, NSLT_Txt_Y, txt, NSLT_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, NSLT_Cir_Y-8, 60, 18, NSLT_Color);
  fillCircle(XRED, NSLT_Cir_Y, Circle_Size, ST7735_RED);
  drawCircle(XYEL, NSLT_Cir_Y, Circle_Size, ST7735_YELLOW);
  drawCircle(XGRN, NSLT_Cir_Y, Circle_Size, ST7735_GREEN);  
  drawtext  (XCNT, NSLT_Count_Y, NSLT_Count, NSLT_Color, ST7735_BLACK, TS_2);
  
  /* EAST/WEST PEDESTRIAM UPDATE FIELD */  
  drawtext(2, PED_EW_Count_Y, PED_EW_Count, EW_Color, ST7735_BLACK, TS_2);
  strcpy(txt, "PEW");  
  drawtext(3, EW_Txt_Y, txt, EW_Color, ST7735_BLACK, TS_1);

  /* NORTH/SOUTH PEDESTRIAM UPDATE FIELD */
  strcpy(txt, "PNS");  
  drawtext(3, NS_Txt_Y, txt, NS_Color, ST7735_BLACK, TS_1);
  drawtext(2, PED_NS_Count_Y, PED_NS_Count, NS_Color, ST7735_BLACK, TS_2);
    
  /* MISCELLANEOUS UPDATE FIELD */  
  strcpy(txt, "EWP EWLT NSP NSLT MR");
  drawtext(1,  Switch_Txt_Y, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(6,  Switch_Txt_Y+9, SW_EWPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(32, Switch_Txt_Y+9, SW_EWLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(58, Switch_Txt_Y+9, SW_NSPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(87, Switch_Txt_Y+9, SW_NSLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  
  drawtext(112,Switch_Txt_Y+9, SW_MODE_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
}
void update_LCD_color(char direction,char color) //code provided by professor
{
     char Circle_Y;
     Circle_Y = EW_Circ_Y + direction * 30;
     if (color == Color_Off) //if Color off make all circles black but leave outline
     {
         fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
         fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
         fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK);
         drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);
         drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
         drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);
     } 
     if (color == Color_Red)
     {
         fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
         fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
         fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK);
         fillCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);
         drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
         drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);
     }
     if (color == Color_Green)
     {
         fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
         fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
         fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK);
         drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);
         drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
         fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);
     }
     if (color == Color_Yellow)
     {
         fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
         fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
         fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK);
         drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);
         fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
         drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);
         
     }

}
void update_LCD_count(char direction, char count)
{
    switch (direction) //update traffic light no ped time
 {
    case EW: //if char (direction == 0 we go to east west )
        EW_Count[0] = count/10 + '0';
        EW_Count[1] = count%10 + '0';
        drawtext(XCNT, EW_Count_Y, EW_Count, EW_Color, ST7735_BLACK, TS_2);
        break;
    case EWLT: // if char (direction) == 1 go to east west left turn
        EWLT_Count[0] = count/10 + '0';
        EWLT_Count[1] = count%10 + '0';
        drawtext(XCNT, EWLT_Count_Y, EWLT_Count, EWLT_Color, ST7735_BLACK, TS_2);
        break;
    case NS: /// if char(direction) == 2 go to north south
        NS_Count[0] = count/10 + '0';
        NS_Count[1] = count%10 + '0';
        drawtext(XCNT, NS_Count_Y, NS_Count, NS_Color, ST7735_BLACK, TS_2);
        break;
    case NSLT: //if char(direction) == 3 go to north south left turn
        NSLT_Count[0] = count/10 + '0';
        NSLT_Count[1] = count%10 + '0';
        drawtext(XCNT, NSLT_Count_Y, NSLT_Count, NSLT_Color, ST7735_BLACK, TS_2);
        break;   
 }
    
}
void update_LCD_PED_Count(char direction, char count)
{
    switch (direction) //select direction and then perform the necessary code
    {
        case EW:
         PED_EW_Count[0] = count/10 + '0'; // PED count upper digit
         PED_EW_Count[1] = count%10 + '0'; // PED Lower
         drawtext(PED_Count_X, PED_EW_Count_Y, PED_EW_Count, EW_Color,
        ST7735_BLACK, TS_2); //Put counter digit on screen
         break; 
       case NS:
         PED_NS_Count[0] = count/10 + '0'; // PED count upper digit
         PED_NS_Count[1] = count%10 + '0'; // PED Lower
         drawtext(PED_Count_X, PED_NS_Count_Y, PED_NS_Count, NS_Color,
        ST7735_BLACK, TS_2); //Put counter digit on screen
         break;
    }
}
void update_LCD_misc() //code provided by professor
{
char ch = 0;
 ADCON0 = ch*4+1; // channel AN0
 int nStep = Get_Full_ADC(); // calculates the # of steps for analog conversion
 volt = nStep * 5 /1000.0; // gets the voltage in Volts, using 5V as reference
 printf("Volt = %f \r\n", volt); //print voltage
 SW_MODE = volt < 2.5 ? 1:0; // SW_MODE = 1, Day_mode, SW_MODE = 0 Night
 //if ( volt < 2.5)
 //{
  //   SW_MODE = 1;
 //}
 //else
 //{
  //   SW_MODE = 0;
 //}
 printf("mode = %f \r\n", SW_MODE); //print voltae
 //SW_EWPED = EWPED_SW;
 SW_EWLT = EWLT_SW;
 //SW_NSPED = NSPED_SW;
 SW_NSLT = NSLT_SW;
 

 if (SW_MODE == 0) SW_MODE_Txt[0] = 'N'; else SW_MODE_Txt[0] = 'D';   
 if (EW_PED_SW == 0) SW_EWPED_Txt[0] = '0'; else SW_EWPED_Txt[0] = '1';
 if (SW_EWLT == 0) SW_EWLT_Txt[0] = '0'; else SW_EWLT_Txt[0] = '1';
 if (NS_PED_SW == 0) SW_NSPED_Txt[0] = '0'; else SW_NSPED_Txt[0] = '1';
 if (SW_NSLT == 0) SW_NSLT_Txt[0] = '0'; else SW_NSLT_Txt[0] = '1';
 if (FLASHING_REQUEST == 0) FlashingR_Txt[0] = '0'; else FlashingR_Txt[0] = '1';
 if (FLASHING == 0) FlashingS_Txt[0] = '0'; else FlashingS_Txt[0] = '1'; 
 drawtext(70, 10, FlashingR_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
 drawtext(100, 10, FlashingS_Txt, ST7735_WHITE, ST7735_BLACK, TS_1); 
 drawtext(6, Switch_Txt_Y+9, SW_EWPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
 drawtext(32, Switch_Txt_Y+9, SW_EWLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
 drawtext(58, Switch_Txt_Y+9, SW_NSPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
 drawtext(87, Switch_Txt_Y+9, SW_NSLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
 drawtext(112, Switch_Txt_Y+9, SW_MODE_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);  

}
void Do_Init()
{
    init_UART();
    init_ADC();
    init_TRIS();
    Initialize_Screen();
    OSCCON = 0x70;
    RBPU = 0x00;
    INTCONbits.INT0IF = 0 ; // Clear INT0IF
    INTCON3bits.INT1IF = 0; // Clear INT1IF
    INTCON3bits.INT2IF =0; // Clear INT2IF
    INTCON2bits.INTEDG0=0 ; // INT0 EDGE falling
    INTCON2bits.INTEDG1=0; // INT1 EDGE falling
    INTCON2bits.INTEDG2=0; // INT2 EDGE falling
    INTCONbits.INT0IE =1; // Set INT0 IE
    INTCON3bits.INT1IE=1; // Set INT1 IE
    INTCON3bits.INT2IE=1; // Set INT2 IE
    INTCONbits.GIE=1; // Set the Global Interrupt Enable
}
void INT0_ISR()
{
 INTCONbits.INT0IF=0; // Clear the interrupt flag
 if (MODE == 1)
 {
 NS_PED_SW = 1; // set software INT0_flag
 }
 else{
     NS_PED_SW =0;
 }
}
void INT1_ISR()
{
 INTCON3bits.INT1IF=0; // Clear the interrupt flag
 if (MODE ==1)
 {
    EW_PED_SW = 1; // set software INT1_flag
 }
 else
 {
     EW_PED_SW = 0;
 }
}
void INT2_ISR()
{
 INTCON3bits.INT2IF=0; // Clear the interrupt flag
 FLASHING_REQUEST = 1;
 //Do_Flashing();
}
void Do_Flashing()
{
    FLASHING = 1;
    while (FLASHING ==1)
    {
        if (FLASHING_REQUEST == 0)
        {
            Set_NS(RED);
            Set_NSLT(RED);
            Set_EW(RED);
            Set_EWLT(RED);
            Wait_One_Second();
            Set_NS(OFF);
            Set_NSLT(OFF);
            Set_EW(OFF);
            Set_EWLT(OFF);
            Wait_One_Second();
            
            //FLASHING_REQUEST = 0;
            //FLASHING = 0;
        }
        else
        {
            FLASHING_REQUEST = 0;
            FLASHING = 0; 
//            Set_NS(RED);
//            Set_NSLT(RED);
//            Set_EW(RED);
//            Set_EWLT(RED);
//            Wait_One_Second();
//            Set_NS(OFF);
//            Set_NSLT(OFF);
//            Set_EW(OFF);
//            Set_EWLT(OFF);
//            Wait_One_Second();
            
        }
    }
}
void interrupt high_priority chkisr()
{
    if (INTCONbits.INT0IF == 1) INT0_ISR(); // check if INT0 // has occured
    if (INTCON3bits.INT1IF == 1) INT1_ISR();
    if (INTCON3bits.INT2IF == 1) INT2_ISR(); 
}
