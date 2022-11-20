; THIS FIRST ASSEMBLY LANGUAGE PROGRAM WILL FLASH AN LED CONNECTED
; TO THE PINS 0 THROUGH 3 OF PORT C
#include<P18F4620.inc>
config OSC = INTIO67
config WDT = OFF
config LVP = OFF
config BOREN = OFF
; Constant declarations
Delay1 equ 0xFF
Delay2 equ 0xFF
Input_A equ 0x20
Input_C equ 0x21
Result equ 0x22
ORG 0x0000
 ; CODE STARTS FROM THE NEXT LINE
START:
 MOVLW 0x0F ; Load W with 0x0F0
 MOVWF ADCON1 ; Make ADCON1 to be all digital
 MOVLW 0xFF ; Load W with 0xFF
 MOVWF PORTA ; Make port A all inputs
 MOVLW 0x00 ; Load W with 0x00
 MOVWF TRISB ; Make PORT B as outputs 
 MOVLW 0xFF ; Load W with  0xFF
 MOVWF TRISC ; Make Port C inputs
 MOVLW 0x07 ; Load W with 0x07
 MOVWF TRISD ; Make Port D both inputs and outputs
 MOVLW 0x00 ; Load W with 0x00
 MOVWF TRISE ; Make PORTE all outputs.
 MAIN_LOOP:
    BTFSC PORTD, 2; Skips if 0
    GOTO PORTD2EQ1 ; Go to EQ1 if current bit is 1.
    GOTO PORTD2EQ0 ; Go to EQ0 if bit was 0
    PORTD2EQ1: 
	GOTO TASK_COMP; If bit eq1 GO TO COMPARISON SUBROUTINE
    PORTD2EQ0:
	BTFSC PORTD,1 ; MOST SIGNIFICANT BIT IS A 1 GO TO PORTD21EQ01 TO DETERMINE NEXT BIT  OTHERWISE GO TO PORTD21EQ00
	GOTO PORTD21EQ01;  GO TO PORTD21EQ01 TO DETERMINE BIT 0'S VALUE
	GOTO PORTD21EQ00; GO TO PORTD21EQ00 TO DETERMINE BIT 0'S VALUE (WHILE BIT 1'S VALUE IS 0)
   
    PORTD21EQ01:
	BTFSC PORTD,0 ; IF LEAST SIGNIFICANT BIT IS 0, GO TO AND, OTHERWISE GO TO XOR
	GOTO TASK_XOR; GO TO TASK_XOR TO SET RGB LED AND START THE OPERATION OF XOR
	GOTO TASK_AND; GO TO TASK_AND TO SET RGB LED AND START THE OPERATION OF AND
    PORTD21EQ00:
	BTFSC PORTD,0;IF BIT 0 IS 0 GO TO TASK_COMP OTHERWISE GO TO TASK_ADD
	GOTO TASK_ADD ; GO TO TASK_ADD TO SET RGB LED AND START THE OPERATION OF ADD
	GOTO TASK_COMP; GO TO TASK_COMP TO SET THE RGB LED AND START THE OPERATION OF COMP.
    TASK_COMP:
	BCF PORTD, 7; CLEAR BIT 7 BECAUSE IT IS A 0 IN 000 (0 IN BINARY)
	BCF PORTD, 6; CLEAR BIT 6 BECAUSE IT IS A 0 IN 000 (0 IN BINARY)
	BCF PORTD, 3; CLEAR BIT 3 BECAUSE IT IS A 0 IN 000 (0 IN BINARY)
	CALL SUBROUTINE_COMP;AFTER SETTING RGB LED GO TO COMP OPERATION
	GOTO MAIN_LOOP;RETURN TO MAIN LOOP SO THAT IT IS AN INFINITE LOOP
    TASK_ADD: 
	BCF PORTD, 7;  CLEAR BIT 7 BECAUSE IT IS A 0 IN 001 (1 IN BINARY)
	BCF PORTD, 6; CLEAR BIT 6 BECAUSE IT IS A 0 IN 001 ( 1 IN BINARY)
	BSF PORTD, 3; SET BIT 3 TO A 1 BECAUSE IT IS A 1 IN 001 (1 IN BINARY)
	CALL SUBROUTINE_ADD; AFTER SETTING RGB LED GO TO ADD OPERATION
	GOTO MAIN_LOOP;;RETURN TO MAIN LOOP SO THAT IT IS AN INFINITE LOOP
    TASK_AND:
	BCF PORTD, 7; CLEAR BIT 7 BECAUSE BIT 7 IS A 0  IN 010
	BSF PORTD, 6; SET BIT 6 TO 1 BECAUSE IT IS A 1 IN 010 (2 IN BINARY)
	BCF PORTD, 3; CLEAR BIT 3 BECAUSE BIT 3 IS A 0 IN 010
	CALL SUBROUTINE_AND; AFTER SETTING RGB LED GO TO AND OPERATION
	GOTO MAIN_LOOP;RETURN TO MAIN LOOP SO THAT IT IS AN INFINITE LOOP
    TASK_XOR:
	BCF PORTD, 7; ; CLEAR BIT 7 BECAUSE BIT 7 IS A 0 IN 011 (3 IN BINARY)
	BSF PORTD, 6; sET TO 1 BECAUSE XOR IS 011
	BSF PORTD, 3; SET TO 1 BECAUSE BIT 3 CORRESPONDS TO THE LAST 1 OF 011
	CALL SUBROUTINE_XOR; AFTER SETTING LED GO TO XOR OPERATION
	GOTO MAIN_LOOP; RETURN TO MAIN LOOP SO THAT IT IS AN INFINITE LOOP
    SUBROUTINE_COMP:
        MOVF PORTA, W ;Read from PORTA and move into W
	ANDLW 0x0F ; Mask with 0x0F
	MOVWF Input_A ; Store into input A 
	COMF Input_A, 0 ; COMPLEMENT of INput A. Store back into W reg.
	ANDLW 0x0F ; Mask off upper four bits.
	MOVWF Result ; Store the complement into the result
	MOVFF Result, PORTB ; Output the result to PORTB
	;BCF PORTE, 1 ; Clear PORTE Bit 1.
	;BNZ CHECK_NOTZ ; Branch not zero. Check WREG if All bits = 0 If not zero->go to Check_NOTZ
	;BSF PORTE, 1 ; If all content is 0. PortE gets 1.
	RETURN
    SUBROUTINE_ADD:
        MOVF PORTA, W ;Read from PORTA and move into W
	ANDLW 0x0F ; Mask with 0x0F
	MOVWF Input_A ; Store into input A 
	MOVF PORTC, W ; Read from PORTC move into W
	ANDLW 0x0F ; Mask with 0x0F
	MOVWF Input_C ; Store into Input C
	MOVF Input_C, W ;;Read From Input_C move into W.
	ADDWF Input_A, 0 ; W has input C, so we add input A
	MOVWF Result ;; Store the result into A.
	MOVFF Result, PORTB ; Output the result to PORTB
	MOVLW 0x00;
	
	;BCF PORTE, 1 ; Clear PORTE Bit 1.
	;BNZ CHECK_NOTZ ; Branch not zero. Check WREG if All bits = 0 If not zero->go to Check_NOTZ
	;BSF PORTE, 1 ; If all content is 0. PortE gets 1.
	RETURN
    SUBROUTINE_AND:
        MOVF PORTA, W ;Read from PORTA and move into W
	ANDLW 0x0F ; Mask with 0x0F
	MOVWF Input_A ; Store into input A 
	MOVF PORTC, W ; Read from PORTC move into W
	ANDLW 0x0F ; Mask with 0x0F
	MOVWF Input_C ; Store into Input C
	MOVF Input_C, W ;;Read From Input_C move into W.
	ANDWF Input_A, 0 ; W has input C, so we AND w reg w/ input A
	MOVWF Result ;; Store the result into A.
	MOVFF Result, PORTB ; Output the result to PORTB
	;BCF PORTE, 1 ; Clear PORTE Bit 1.
	;BNZ CHECK_NOTZ ; Branch not zero. Check WREG if All bits = 0 If not zero->go to Check_NOTZ
	;BSF PORTE, 1 ; If all content is 0. PortE gets 1.
	RETURN
    SUBROUTINE_XOR:
        MOVF PORTA, W ;Read from PORTA and move into W
	ANDLW 0x0F ; Mask with 0x0F
	MOVWF Input_A ; Store into input A 
	MOVF PORTC, W ; Read from PORTC move into W
	ANDLW 0x0F ; Mask with 0x0F
	MOVWF Input_C ; Store into Input C
	MOVF Input_C, W ;;Read From Input_C move into W.
	XORWF Input_A, 0 ; W has input C, so we XOR w reg w/ input A
	MOVWF Result ;; Store the result into A.
	MOVFF Result, PORTB ; Output the result to PORTB
	;BCF PORTE, 1 ; Clear PORTE Bit 1.
	;BNZ CHECK_NOTZ ; Branch not zero. Check WREG if All bits = 0 If not zero->go to Check_NOTZ
	;BSF PORTE, 1 ; If all content is 0. PortE gets 1.
	RETURN
    ;;CHECK_NOTZ:
    ;;GOTO MAIN_LOOP
 END
