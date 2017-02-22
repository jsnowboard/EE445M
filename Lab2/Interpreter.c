// Interpreter.c
// Jeremiah Bartlett
// jjb2954

#include <stdint.h>
#include "UART2.h"
#include "ADC.h"
#include "OS.h"
#include "ST7735.h"

//---------------------OutCRLF---------------------
// Output a CR,LF to UART to go to a new line
// Input: none
// Output: none
void OutCRLF(void){
  UART_OutChar(CR);
  UART_OutChar(LF);
}

int i = 0, kill = 0, linet = 0, lineb = 0;

void Interpreter()
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
		  value = ADC_In();
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
		  // TODO: ADC_Collect(0, value, valueBuf, val2);
      for (i = 0; i<val2; i++)
		  {
				UART_OutUDec(valueBuf[i]);
			}
			OutCRLF();
		case 't':
		case 'T':
			UART_OutString("Timer Value: ");
		  value = OS_Time();
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

void Jitter(void)
{
	// TODO: Write a function that prints out the jitter
}
