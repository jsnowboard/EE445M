// adc.c
// Jeremiah Bartlett
// jjb2954

#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
#include "ADC.h"

int value;

#define PE3 0

void ADC_Init(uint32_t bmask)
{
	SYSCTL_RCGC0_R |= 0x00010000; //ADC0
}

int ADC_Open(uint32_t channelNum)
{
	return 0;
}

uint16_t ADC_In(void)
{
	return PE3;
}

void ADC_Collect(uint32_t channelNum, uint32_t fs, void(task(unsigned long data)))
{
	uint32_t value;
	ADC_Init(channelNum);
	
	task(value);
	
}

int ADC_Status(void)
{
	return 0;
}
