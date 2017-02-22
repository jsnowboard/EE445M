// UARTIntsTestMain.c
// Runs on LM4F120/TM4C123
// Tests the UART0 to implement bidirectional data transfer to and from a
// computer running HyperTerminal.  This time, interrupts and FIFOs
// are used.
// Daniel Valvano
// September 12, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
   Program 5.11 Section 5.6, Program 3.10

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// U0Rx (VCP receive) connected to PA0
// U0Tx (VCP transmit) connected to PA1

#include <stdint.h>
#include "PLL.h"
#include "UART.h"

#include "ST7735.h"
#include "ADCSWTrigger.h"
#include "OS.h"
#include "Timer2.h"
#include "../inc/tm4c123gh6pm.h"


#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define RED       0x02

#define LEDS      (*((volatile uint32_t *)0x40025038))

const uint32_t COLORWHEEL[2] = {RED, 0};
//---------------------OutCRLF---------------------
// Output a CR,LF to UART to go to a new line
// Input: none
// Output: none
void OutCRLF(void){
  UART_OutChar(CR);
  UART_OutChar(LF);
}
// NEW: Lab 1

int kill = 0, linet = 0, lineb = 0;

void interpret()
{
	char buffer[20];
	char message[40];
	int value = 0, val2 = 0;
	uint16_t* valueBuf;
	
	UART_OutString("Enter a Command: ");
	UART_InString(buffer,19);
	OutCRLF();
	switch (buffer[0])
	{
		case 'p':	
    case 'P':	
  		UART_OutString("InString: ");
      UART_InString(message,39);
	    OutCRLF();
		  ST7735_Message(1,lineb, message, -1);
		  lineb++;
		  if (lineb>=3) { lineb = 0; }
		  break;
		case 'c':
		case 'C':
			UART_OutString("ADC Value: ");
		  value = ADC0_InSeq3();
		  UART_OutUDec(value);// Output the ADC Value
		  OutCRLF();
  		ST7735_Message(0, 0, "ST7735:\nADC Value=", value);
		  break;
		case 'd':
			UART_OutString("Enter Frequency: ");
		  value = UART_InUDec();
		  OutCRLF();
		  UART_OutString("Enter number of Samples: ");
		  val2 = UART_InUDec();
		  if(val2>100) {val2 = 0; } // take this out and make it better
		  OutCRLF();
		  ADC_Collect(0, value, valueBuf, val2);
      for (int i = 0; i<val2; i++)
		  {
				UART_OutUDec(valueBuf[i]);
			}
			OutCRLF();
		case 't':
		case 'T':
			UART_OutString("Timer Value: ");
		  value = OS_ReadPeriodicTime();
		  UART_OutUDec(value);// Output the timer value
		  OutCRLF();
  		ST7735_Message(0, 2, "Timer Value=", value);
		  break;
		case 'e':
		case 'E':
			kill = 1;
		  break;
		default:
			break;
	}
	
}

// LCD Driver Test
int main(void)
{
	char top[] = "Print on top";
	char bottom[] = "Print on botom";
	int channelNum = 0; // ADC Channel 0, PE3
	volatile uint32_t delay;
	
  PLL_Init(Bus80MHz); 
		
  SYSCTL_RCGCGPIO_R |= 0x20;       // activate port F
  delay = SYSCTL_RCGCGPIO_R;       // allow time to finish activating
  GPIO_PORTF_DIR_R |= 0x0E;        // make PF3-1 output (PF3-1 built-in LEDs)
  GPIO_PORTF_AFSEL_R &= ~0x0E;     // disable alt funct on PF3-1
  GPIO_PORTF_DEN_R |= 0x0E;        // enable digital I/O on PF3-1
                                   // configure PF3-1 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFF000F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;          // disable analog functionality on PF
	UART_Init();
	OutCRLF();
	ST7735_InitR(INITR_GREENTAB);
	Output_Init();
	//ADC_Init(channelNum);
	ADC0_InitSWTriggerSeq3(channelNum);
	
  SYSCTL_RCGCGPIO_R |= 0x20;       // activate port F
  delay = SYSCTL_RCGCGPIO_R;       // allow time to finish activating
  GPIO_PORTF_DIR_R |= 0x0E;        // make PF3-1 output (PF3-1 built-in LEDs)
  GPIO_PORTF_AFSEL_R &= ~0x0E;     // disable alt funct on PF3-1
  GPIO_PORTF_DEN_R |= 0x0E;        // enable digital I/O on PF3-1
                                   // configure PF3-1 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFF000F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;          // disable analog functionality on PF
	LEDS = 0;
	
	
	OS_AddPeriodicThread(&perTask, 80000000, 4);
	while (!kill)
	{
		interpret();
	}
	
	Output_Clear();
	ST7735_Message(0, 0, top, 0);
	ST7735_Message(0, 1, "2nd Line", 0);
	ST7735_Message(0, 2, "3rd Line", 0);
	ST7735_Message(0, 3, "Last line", 0);
	ST7735_Message(1, 0, bottom, 0);
	ST7735_Message(1, 1, "2nd Line", 0);
}

//debug code
//int main(void){
//  char i;
//  char string[20];  // global to assist in debugging
//  uint32_t n;

//  PLL_Init();               // set system clock to 50 MHz
//  UART_Init();              // initialize UART
//  OutCRLF();
//  for(i='A'; i<='Z'; i=i+1){// print the uppercase alphabet
//    UART_OutChar(i);
//  }
//  OutCRLF();
//  UART_OutChar(' ');
//  for(i='a'; i<='z'; i=i+1){// print the lowercase alphabet
//    UART_OutChar(i);
//  }
//  OutCRLF();
//  UART_OutChar('-');
//  UART_OutChar('-');
//  UART_OutChar('>');
//  while(1){
//    UART_OutString("InString: ");
//    UART_InString(string,19);
//    UART_OutString(" OutString="); UART_OutString(string); OutCRLF();

//    UART_OutString("InUDec: ");  n=UART_InUDec();
//    UART_OutString(" OutUDec="); UART_OutUDec(n); OutCRLF();

//    UART_OutString("InUHex: ");  n=UART_InUHex();
//    UART_OutString(" OutUHex="); UART_OutUHex(n); OutCRLF();

//  }
//}
