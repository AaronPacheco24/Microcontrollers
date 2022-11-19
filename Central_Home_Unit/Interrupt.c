#include <xc.h>
#include <p18f4620.h>
#include "Interrupt.h"
#include "stdio.h"

unsigned char bit_count;
unsigned int Time_Elapsed;

extern unsigned char Nec_state;
extern short nec_ok;
unsigned long long Nec_code;

extern char Nec_code1;
extern char INT1_flag;
extern char INT2_flag;

void Init_Interrupt(void)
{
                                                    // initialize the INT0, INT1, INT2 
    INTCONbits.INT0IF = 0 ;                         // interrupts
    INTCON3bits.INT1IF = 0;             
    INTCON3bits.INT2IF = 0;            
    INTCONbits.INT0IE = 1;           
    INTCON3bits.INT1IE = 1;            
    INTCON3bits.INT2IE = 1;           
    INTCON2bits.INTEDG0 = 0;         
    INTCON2bits.INTEDG1 = 0;          
    INTCON2bits.INTEDG2 = 0;  
    TMR1H = 0;                                      // Reset Timer1
    TMR1L = 0;                                      //
    PIR1bits.TMR1IF = 0;                            // Clear timer 1 interrupt flag
    PIE1bits.TMR1IE = 1;                            // Enable Timer 1 interrupt
    INTCONbits.PEIE = 1;                            // Enable Peripheral interrupt
    INTCONbits.GIE = 1;                             // Enable global interrupts

}

void interrupt  high_priority chkisr() 
{    
	// add code here to handle TIMER1_isr(), INT0_isr() INT1_isr() and INT2_isr()
    if (PIR1bits.TMR1IF == 1) TIMER1_isr(); //One interrupt service routine trigger
    if (INTCONbits.INT0IF == 1) INT0_isr(); // check if INT0 // has occured
    if (INTCON3bits.INT1IF == 1) INT1_isr(); //
   // if (INTCON3bits.INT2IF == 1) INT2_isr(); 
}

void TIMER1_isr(void)
{
    Nec_state = 0;                          // Reset decoding process
    INTCON2bits.INTEDG0 = 0;                // Edge programming for INT0 falling edge
    T1CONbits.TMR1ON = 0;                   // Disable T1 Timer
    PIR1bits.TMR1IF = 0;                    // Clear interrupt flag
}

void force_nec_state0()
{
    Nec_state=0;
    T1CONbits.TMR1ON = 0;
}

void INT0_isr() 
{    
     INTCONbits.INT0IF = 0;                  // Clear external interrupt
    
    if (Nec_state != 0)
    {
        Time_Elapsed = (TMR1H << 8) | TMR1L;       // Store Timer1 value
        TMR1H = 0;                          // Reset Timer1
        TMR1L = 0;
    }
    
    switch(Nec_state)
    {
        case 0 :
        {
                                            // Clear Timer 1
            TMR1H = 0;                      // Reset Timer1
            TMR1L = 0;                      //
            PIR1bits.TMR1IF = 0;            //
            T1CON= 0x90;                    // Program Timer1 mode with count = 1usec using System clock running at 8Mhz
            T1CONbits.TMR1ON = 1;           // Enable Timer 1
            bit_count = 0;                  // Force bit count (bit_count) to 0
            Nec_code = 0;                   // Set Nec_code = 0
            Nec_state = 1;                  // Set Nec_State to state 1
            INTCON2bits.INTEDG0 = 1;        // Change Edge interrupt of INT0 to Low to High      
        
            return;
        }
        
        case 1 :
        {
            
            if ((Time_Elapsed > 8500) && (Time_Elapsed <9500))
            {
                Nec_state = 2; //set nec sate to 2
                //INTCON2bits.INTEDG0 = 0; //Change interrupt
                
            }
            else
            {
                force_nec_state0();
            }
            INTCON2bits.INTEDG0 = 0; //Change interrupt
            return;
        }
        
        case 2 :                            // Add your code here
        {
            
            if ((Time_Elapsed > 4000) && (Time_Elapsed <5000))
            {
                Nec_state = 3; //set nec sate to 2
             //   INTCON2bits.INTEDG0 = 1; //Change interrupt
            }
            else
            {
                force_nec_state0();
            }
            INTCON2bits.INTEDG0 = 1; //Change interrupt
            return;
        }
        
        case 3 :                            // Add your code here
        {

            if ((Time_Elapsed > 400) && (Time_Elapsed <700))
            {
                ///printf("hello");
                Nec_state =4; //set nec sate to 2
 
                
            }
            else
            {
                force_nec_state0();
 
            }
            INTCON2bits.INTEDG0 = 0; //Change interrupt
            return;
        }
        
        case 4 :                            // Add your code here
        {
            if ((Time_Elapsed > 400) && (Time_Elapsed <1800))
            {
                Nec_code=Nec_code<<1; //shift left 1 bit
                if (Time_Elapsed > 1000)
                {
                    Nec_code=Nec_code +1; //if its within this period, add one to the nec code
                }
                bit_count = bit_count+1; //add one to bitcount
                if (bit_count>31) //if the bitcount is over 31
                {
                    nec_ok = 1; //ok = 1
                    Nec_code1 = (char) ((Nec_code >> 8));  //this is a global variable so we dont send a large variable to the main . This is nec code shifted left 8 times for the found key
                    INTCONbits.INT0IE = 0; //clear the bit
                    Nec_state = 0; //nec state set to 0
                    
                }
                Nec_state=3; //set nec state to 3 so we stay here
            }
            else
            {
                force_nec_state0();   //otherwise set the nec state to 0 to break out
            }
            INTCON2bits.INTEDG0 = 1; //set to low to high
            return;
        }
    }                            

 	// add old code for Infrared 
    
}

void INT1_isr() 
{  
    INTCON3bits.INT1IF = 0; //clear hardware flag to get out of ISR quickly
    INT1_flag = 1; //Set software flag
    

} 
