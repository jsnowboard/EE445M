// OS.C
// Jeremiah Bartlett
// jjb2954

#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
#include "Timer2.h"
#include "OS.h"
#include "PLL.h"
#include "UART2.h"

#define NUM_THREADS 9
#define STACKSIZE 100

struct tcb
{
	uint32_t* sp;
	struct tcb* nextPtr;
	struct tcb* prevPtr;
	uint32_t Id;
	uint32_t sleepVal; // add sleep state flag val- how long to sleep
	uint8_t priority;
	int blocked; // add blocked state-bool
};
typedef struct tcb tcbType;
tcbType tcbList[NUM_THREADS];
tcbType *RunPt;
tcbType *NextRunPt;

uint32_t Stacks[NUM_THREADS][STACKSIZE];

// Critical Section functions
int32_t StartCritical(void);
void EndCritical(int32_t primask);
void OS_DisableInterrupts(void);
void OS_EnableInterrupts(void);
void StartOS(void);


void SetInitialStack(int i)
{
	tcbList[i].sp = &Stacks[i][STACKSIZE-16];// thread stack pointer
  Stacks[i][STACKSIZE-1] = 0x01000000;   // thumb bit
  Stacks[i][STACKSIZE-3] = 0x14141414;   // R14
  Stacks[i][STACKSIZE-4] = 0x12121212;   // R12
  Stacks[i][STACKSIZE-5] = 0x03030303;   // R3
  Stacks[i][STACKSIZE-6] = 0x02020202;   // R2
  Stacks[i][STACKSIZE-7] = 0x01010101;   // R1
  Stacks[i][STACKSIZE-8] = 0x00000000;   // R0
  Stacks[i][STACKSIZE-9] = 0x11111111;   // R11
  Stacks[i][STACKSIZE-10] = 0x10101010;  // R10
  Stacks[i][STACKSIZE-11] = 0x09090909;  // R9
  Stacks[i][STACKSIZE-12] = 0x08080808;  // R8
  Stacks[i][STACKSIZE-13] = 0x07070707;  // R7
  Stacks[i][STACKSIZE-14] = 0x06060606;  // R6
  Stacks[i][STACKSIZE-15] = 0x05050505;  // R5
  Stacks[i][STACKSIZE-16] = 0x04040404;  // R4
}

void periodicTask(void)
{
	// periodic task to be done when timer counts down
}


// ******** OS_Init ************
// initialize operating system, disable interrupts until OS_Launch
// initialize OS controlled I/O: serial, ADC, systick, LaunchPad I/O and timers 
// input:  none
// output: none
void OS_Init(void)
{
	// Disable interrupts
	OS_DisableInterrupts();
	PLL_Init(Bus80MHz); 
	UART_Init();
	
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_CURRENT_R = 0;
															// lowest PRI so only foreground interrupted
  NVIC_SYS_PRI3_R =(NVIC_SYS_PRI3_R&0x00FFFFFF)|0xE0000000; // priority 7
	
	NVIC_INT_CTRL_R = 0;
	
	// PendSV to trigger context switch
	NVIC_SYS_PRI3_R =(NVIC_SYS_PRI3_R&0xFF00FFFF)|0x00D00000; // priority 6	
	
}

// ******** OS_InitSemaphore ************
// initialize semaphore 
// input:  pointer to a semaphore
// output: none
void OS_InitSemaphore(Sema4Type *semaPt, long value)
{
	semaPt->Value = value;
}
// ******** OS_Wait ************
// decrement semaphore 
// Lab2 spinlock
// Lab3 block if less than zero
// input:  pointer to a counting semaphore
// output: none
void OS_Wait(Sema4Type *semaPt)
{
	OS_DisableInterrupts();
	while (semaPt->Value <= 0)
	{
		OS_EnableInterrupts();
		OS_Suspend();           // coop multithread
		OS_DisableInterrupts();
	}
	semaPt->Value -= 1;
	OS_EnableInterrupts();// 
		
}

// ******** OS_Signal ************
// increment semaphore 
// Lab2 spinlock
// Lab3 wakeup blocked thread if appropriate 
// input:  pointer to a counting semaphore
// output: none
void OS_Signal(Sema4Type *semaPt)
{
	int sr;
	sr = StartCritical();
	semaPt->Value += 1;
	EndCritical(sr);
	
}

// ******** OS_bWait ************
// Lab2 spinlock, set to 0
// Lab3 block if less than zero
// input:  pointer to a binary semaphore
// output: none
void OS_bWait(Sema4Type *semaPt)
{
	OS_DisableInterrupts();
	while(semaPt->Value == 0)
	{
		OS_EnableInterrupts();
		OS_Suspend();
		OS_DisableInterrupts();
	}
	semaPt->Value = 0;
	OS_EnableInterrupts();
		
}

// ******** OS_bSignal ************
// Lab2 spinlock, set to 1
// Lab3 wakeup blocked thread if appropriate 
// input:  pointer to a binary semaphore
// output: none
void OS_bSignal(Sema4Type *semaPt)
{
	int sr;
	sr = StartCritical();
	semaPt->Value = 1;
	EndCritical(sr);
	
}

//******** OS_AddThread *************** 
// add a foregound thread to the scheduler
// Inputs: pointer to a void/void foreground task
//         number of bytes allocated for its stack
//         priority, 0 is highest, 5 is the lowest
// Outputs: 1 if successful, 0 if this thread can not be added
// stack size must be divisable by 8 (aligned to double word boundary)
// In Lab 2, you can ignore both the stackSize and priority fields
// In Lab 3, you can ignore the stackSize fields
static uint32_t numThreads;
int OS_AddThread(void(*task)(void), 
   unsigned long stackSize, unsigned long priority)
{
	int sr, i, success = 0;
	if (stackSize % 8)
	{
		return 0; //stack size not valid
	}
	// start critical section here, should not interrupt
	// until the thread is fully added
	
	sr = StartCritical();
	
	for (i = numThreads; i < NUM_THREADS; i ++)
	{
		if (tcbList[i].Id == 0)
		{
			// {2, 3, -1, 5}
			// {X, A, Z, B) 
			numThreads++;                                // maybe not needed? good for debug
			tcbList[i].Id = numThreads;                 
			tcbList[i].priority = priority;              
			tcbList[i].sleepVal = 0;
			tcbList[i].blocked = 0;
			SetInitialStack(i);                          // set up initial stack and set the stack pointer;
			Stacks[i][STACKSIZE-2] = (int32_t)(task);
			
			tcbList[i].nextPtr = &tcbList[0];            // Z.nextPtr = &B
			tcbList[i].prevPtr = tcbList[0].prevPtr;     // Z.prevPtr = &A
			tcbList[0].prevPtr->nextPtr = &tcbList[i];   // A.nextPtr = &Z
			tcbList[0].prevPtr = &tcbList[i];          // B.prevPtr = &Z
			
			RunPt = &tcbList[0];
			NextRunPt = tcbList[0].nextPtr;
			success = 1;
			break;
		}
	}
	
	EndCritical(sr);
	// end critical section before returning
	return success; 
}

//******** OS_Id *************** 
// returns the thread ID for the currently running thread
// Inputs: none
// Outputs: Thread ID, number greater than zero 
unsigned long OS_Id(void)
{
	unsigned long tId;
	
	tId = RunPt->Id;
	
	return tId;
}

//******** OS_AddPeriodicThread *************** 
// add a background periodic task
// typically this function receives the highest priority
// Inputs: pointer to a void/void background function
//         period given in system time units (12.5ns)
//         priority 0 is the highest, 5 is the lowest
// Outputs: 1 if successful, 0 if this thread can not be added
// You are free to select the time resolution for this function
// It is assumed that the user task will run to completion and return
// This task can not spin, block, loop, sleep, or kill
// This task can call OS_Signal  OS_bSignal	 OS_AddThread
// This task does not have a Thread ID
// In lab 2, this command will be called 0 or 1 times
// In lab 2, the priority field can be ignored
// In lab 3, this command will be called 0 1 or 2 times
// In lab 3, there will be up to four background threads, and this priority field 
//           determines the relative priority of these four threads
int OS_AddPeriodicThread(void(*task)(void), 
   unsigned long period, unsigned long priority)
{
	   Timer2_Init(task, period);
		 return 1; //successful add of this periodic thread
}

//******** OS_AddSW1Task *************** 
// add a background task to run whenever the SW1 (PF4) button is pushed
// Inputs: pointer to a void/void background function
//         priority 0 is the highest, 5 is the lowest
// Outputs: 1 if successful, 0 if this thread can not be added
// It is assumed that the user task will run to completion and return
// This task can not spin, block, loop, sleep, or kill
// This task can call OS_Signal  OS_bSignal	 OS_AddThread
// This task does not have a Thread ID
// In labs 2 and 3, this command will be called 0 or 1 times
// In lab 2, the priority field can be ignored
// In lab 3, there will be up to four background threads, and this priority field 
//           determines the relative priority of these four threads
int prevPF4;
void (*sw1task)(void); // this task will be called
int OS_AddSW1Task(void(*task)(void), unsigned long priority)
{  
  SYSCTL_RCGCGPIO_R |= 0x00000020; // (a) activate clock for port F
  while((SYSCTL_PRGPIO_R&0x20) == 0){};// ready?
	GPIO_PORTF_CR_R |= 0x10;         // allow changes to PF4
  GPIO_PORTF_DIR_R &= ~0x10;      // (c) make PF4 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~0x10;    //     disable alt funct on PF4
  GPIO_PORTF_DEN_R |= 0x10;       //     enable digital I/O on PF4
  GPIO_PORTF_PCTL_R &= ~0;        //     PF4 is not both edges
  GPIO_PORTF_AMSEL_R &= ~0x10;    //     disable analog functionality on PF4
  GPIO_PORTF_PUR_R |= 0x10;       //     enable weak pull-up on PF4
		
  GPIO_PORTF_IS_R &= ~0x10;       // (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R |= 0x10;       // (e) clear flags 4
  GPIO_PORTF_IM_R |= 0x10;        // (f) arm interrupt on PF4
  GPIO_PORTF_IEV_R &= ~0x10;      //     PF4 falling edge event
  GPIO_PORTF_ICR_R = 0x10;        //  configure PF4
  prevPF4 = GPIO_PORTF_DATA_R & 0x10;  // used to debounce

  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 2
  NVIC_EN0_R = 0x40000000;                           // (h) enable interrupt 30 in NVIC
	
	sw1task = task;
	return 1;                                          // successful add
}

//******** OS_AddSW2Task *************** 
// add a background task to run whenever the SW2 (PF0) button is pushed
// Inputs: pointer to a void/void background function
//         priority 0 is highest, 5 is lowest
// Outputs: 1 if successful, 0 if this thread can not be added
// It is assumed user task will run to completion and return
// This task can not spin block loop sleep or kill
// This task can call issue OS_Signal, it can call OS_AddThread
// This task does not have a Thread ID
// In lab 2, this function can be ignored
// In lab 3, this command will be called will be called 0 or 1 times
// In lab 3, there will be up to four background threads, and this priority field 
//           determines the relative priority of these four threads
void (*sw2task)(void);
int OS_AddSW2Task(void(*task)(void), unsigned long priority)
{ 
	//Board_Init();  // Set up board with onboard buttons
	sw2task = task;
	return 1; // successful add
}

// ******** OS_Sleep ************
// place this thread into a dormant state
// input:  number of msec to sleep
// output: none
// You are free to select the time resolution for this function
// OS_Sleep(0) implements cooperative multitasking
void OS_Sleep(unsigned long sleepTime)
{
	// TODO: make sure it sleeps correctly
	if (sleepTime == 0)
	{
		OS_Suspend();
	}
	RunPt->sleepVal = sleepTime;
	
}

// ******** OS_Kill ************
// kill the currently running thread, release its TCB and stack
// input:  none
// output: none
void OS_Kill(void)
{
	RunPt->prevPtr->nextPtr = RunPt->nextPtr;
  RunPt->nextPtr->prevPtr = RunPt->prevPtr;
  RunPt->Id = 0;	
	numThreads --;
	OS_Suspend(); //trigger context switch
}

// ******** OS_Suspend ************
// suspend execution of currently running thread
// scheduler will choose another thread to execute
// Can be used to implement cooperative multitasking 
// Same function as OS_Sleep(0)
// input:  none
// output: none
void OS_Suspend(void)
{
	//NVIC_INT_CTRL_R = 0x04000000;		// trigger SysTick
	NVIC_INT_CTRL_R = 0x10000000;	  // trigger PendSV
}
 
// ******** OS_Fifo_Init ************
// Initialize the Fifo to be empty
// Inputs: size
// Outputs: none 
// In Lab 2, you can ignore the size field
// In Lab 3, you should implement the user-defined fifo size
// In Lab 3, you can put whatever restrictions you want on size
//    e.g., 4 to 64 elements
//    e.g., must be a power of 2,4,8,16,32,64,128
void OS_Fifo_Init(unsigned long size)
{
	
}

// ******** OS_Fifo_Put ************
// Enter one data sample into the Fifo
// Called from the background, so no waiting 
// Inputs:  data
// Outputs: true if data is properly saved,
//          false if data not saved, because it was full
// Since this is called by interrupt handlers 
//  this function can not disable or enable interrupts
int OS_Fifo_Put(unsigned long data)
{
	return 1; //data is saved
}

// ******** OS_Fifo_Get ************
// Remove one data sample from the Fifo
// Called in foreground, will spin/block if empty
// Inputs:  none
// Outputs: data 
unsigned long OS_Fifo_Get(void)
{
	unsigned long data;
	return data;
}

// ******** OS_Fifo_Size ************
// Check the status of the Fifo
// Inputs: none
// Outputs: returns the number of elements in the Fifo
//          greater than zero if a call to OS_Fifo_Get will return right away
//          zero or less than zero if the Fifo is empty 
//          zero or less than zero if a call to OS_Fifo_Get will spin or block
long OS_Fifo_Size(void)
{
	return 1; // number of elements in the fifo
}

// ******** OS_MailBox_Init ************
// Initialize communication channel
// Inputs:  none
// Outputs: none
void OS_MailBox_Init(void)
{
	
}

// ******** OS_MailBox_Send ************
// enter mail into the MailBox
// Inputs:  data to be sent
// Outputs: none
// This function will be called from a foreground thread
// It will spin/block if the MailBox contains data not yet received 
void OS_MailBox_Send(unsigned long data)
{
	
}

// ******** OS_MailBox_Recv ************
// remove mail from the MailBox
// Inputs:  none
// Outputs: data received
// This function will be called from a foreground thread
// It will spin/block if the MailBox is empty 
unsigned long OS_MailBox_Recv(void)
{
	unsigned long data;
	return data;
}

// ******** OS_Time ************
// return the system time 
// Inputs:  none
// Outputs: time in 12.5ns units, 0 to 4294967295
// The time resolution should be less than or equal to 1us, and the precision 32 bits
// It is ok to change the resolution and precision of this function as long as 
//   this function and OS_TimeDifference have the same resolution and precision 
unsigned long OS_Time(void)
{
	unsigned long time;
	return time;
}

// ******** OS_TimeDifference ************
// Calculates difference between two times
// Inputs:  two times measured with OS_Time
// Outputs: time difference in 12.5ns units 
// The time resolution should be less than or equal to 1us, and the precision at least 12 bits
// It is ok to change the resolution and precision of this function as long as 
//   this function and OS_Time have the same resolution and precision 
unsigned long OS_TimeDifference(unsigned long start, unsigned long stop)
{
	unsigned long timeDiff;
	return timeDiff;
}

// ******** OS_ClearMsTime ************
// sets the system time to zero (from Lab 1)
// Inputs:  none
// Outputs: none
// You are free to change how this works
void OS_ClearMsTime(void)
{
	
}

// ******** OS_MsTime ************
// reads the current time in msec (from Lab 1)
// Inputs:  none
// Outputs: time in ms units
// You are free to select the time resolution for this function
// It is ok to make the resolution to match the first call to OS_AddPeriodicThread
unsigned long OS_MsTime(void)
{
	unsigned long time;
	return time;
}

//******** OS_Launch *************** 
// start the scheduler, enable interrupts
// Inputs: number of 12.5ns clock cycles for each time slice
//         you may select the units of this parameter
// Outputs: none (does not return)
// In Lab 2, you can ignore the theTimeSlice field
// In Lab 3, you should implement the user-defined TimeSlice field
// It is ok to limit the range of theTimeSlice to match the 24-bit SysTick
void OS_Launch(unsigned long theTimeSlice)
{
	NVIC_ST_RELOAD_R = theTimeSlice - 1;
	NVIC_ST_CTRL_R = 0x00000007; //Enable, core clock and interrupt arm
	StartOS();
}

//******* OS_Scheduler *************
void Scheduler(void)
{
	RunPt = RunPt->nextPtr;
}


#define PB3  (*((volatile unsigned long *)0x40005020))

//void SysTick_Handler(void) {	
//	PB3 ^= 0x8;
//	NextRunPt = RunPt->nextPtr;
//	
//	while(NextRunPt->sleepVal) {
//		NextRunPt = NextRunPt->nextPtr;
//	}

//  NVIC_INT_CTRL_R = 0x10000000;		// trigger PendSV
//}

void sleeper(void)
{
	int i = 0;
	for(i=0; i<NUM_THREADS; i++)
	{
		if(tcbList[i].sleepVal>0)
		{
			tcbList[i].sleepVal --;
		}
	}
	return;
}
