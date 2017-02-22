// OS.C
// Jeremiah Bartlett
// jjb2954

#include <stdint.h>
#include "OS.h"
#include "Timer2.h"

#include "../inc/tm4c123gh6pm.h"

#define TIMER2_TAV_R            (*((volatile uint32_t *)0x40032050))

int time = 0; //

void perTask()
{
	time++;
}

int OS_AddPeriodicThread(void(*task)(void), uint32_t period, uint32_t priority)
{
	Timer2_Init(task, period);
	return 0;
}

void OS_ClearPeriodicTime()
{
	time = 0;
}

uint32_t OS_ReadPeriodicTime()
{
	return time;
}
