#include <LPC23xx.H>                    /* LPC23xx definitions                */

#include <stdio.h>
#include <stdlib.h>
#include <rtos.h>

#include <lcd.h>
#include "rwLockObject/rwLockObject.h"

#define READER_THREADS	3
#define WRITER_THREADS	3
threadObject_t writerThread[3];
threadObject_t readerThread[3];
void writer(int32 num);
void reader(int32 num);
int32 writerStack[3][1000];
int32 readerStack[3][1000];
int32 share=0;
rwLockObject_t shareLock;
char writerNames[3][10] = {"writer(0)","writer(1)","writer(2)"};
char readerNames[3][10] = {"reader(0)","reader(1)","reader(2)"};

/* Import external functions from Serial.c file                               */
extern       void init_serial    (void);


void LED_Init(void) {
  PINSEL10 = 0;                         /* Disable ETM interface, enable LEDs */
  FIO2DIR  = 0x000000FF;                /* P2.0..7 defined as Outputs         */
  FIO2MASK = 0x00000000;
}

/* Function that outputs value to LEDs                                        */
void LED_Out(unsigned int value) {
  FIO2CLR = 0xFF;                       /* Turn off all LEDs                  */
  FIO2SET = (value & 0xFF);             /* Turn on requested LEDs             */
}

int main(void)
{
	int i=0;
	LED_Init();
	LED_Out(0x55);

	init_serial();
	
	lcd_init();
	lcd_clear();
	lcd_print ("  SROS Lab4  ");
	set_cursor (0, 1);
 	 
	
	rwLockObjectInit(&shareLock);
	
	printf("\n\n\n\n==MAIN==\n");
	lcd_print ("==MAIN==");

	rtosInit();

	set_cursor (0, 1);
	lcd_print ("==DONE INIT==");
    
	for (i=0;i<WRITER_THREADS;++i) {
		threadObjectCreate(
			&(writerThread[i]),
			(void *)writer,
			i,
			0,
			0,
			0,
			&writerStack[i][1000],
			1,
			INITIAL_CPSR_ARM_FUNCTION,
			writerNames[i]
		);
	}
                        
	for (i=0;i<READER_THREADS;++i) {
		threadObjectCreate(
			&(readerThread[i]),
			(void *)reader,
			i,
			0,
			0,
			0,
			&readerStack[i][1000],
			1,
			INITIAL_CPSR_ARM_FUNCTION,
			readerNames[i]
		);
	}
                        
	srand(1);
                        
	//Disable interrupts and setup interrupts.
	/* Enable and setup timer interrupt, start timer                            */
	T0MR0         = 11999;                       /* 1msec = 12000-1 at 12.0 MHz */
	T0MCR         = 3;                           /* Interrupt and Reset on MR0  */
	T0TCR         = 1;                           /* Timer0 Enable               */
	VICVectAddr4  = (unsigned long)irq_interrupt_handler;/* Set Interrupt Vector        */
	VICVectCntl4  = 15;                          /* use it for Timer0 Interrupt */
	VICIntEnable  = (1  << 4);                   /* Enable Timer0 Interrupt     */
	
	scheduler();            //This function will never return.
}                       
                        
                        
void writer(int32 num)
{
	while (1) {
		rwLockObjectLockWriter(&shareLock);
		printf("Writer(%d):%d\n",num,++share);
		rwLockObjectRelease(&shareLock);
	}
}
                    
void reader(int32 num)
{
	while (1) {
		rwLockObjectLockReader(&shareLock);
		printf("Reader(%d):%d\n",num,share);
		rwLockObjectRelease(&shareLock);
	}
}
                    
                                
void irq_interrupt_service_routine(void)
{
    timerTick();
    T0IR        = 1;                      /* Clear interrupt flag               */
    VICVectAddr = 0;                      /* Acknowledge Interrupt              */
    return;
}
