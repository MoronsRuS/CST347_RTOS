#include <LPC23xx.H>                    /* LPC23xx definitions                */

#include <stdio.h>
#include <stdlib.h>
#include <rtos.h>

#include <lcd.h>

threadObject_t thread1, thread2;
void function1(void);
void function2(void);
int32 stack1[1000], stack2[1000];

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
	LED_Init();
	LED_Out(0x55);

	init_serial();
	
	lcd_init();
	lcd_clear();
	lcd_print ("  SROS Lab3  ");
	set_cursor (0, 1);
 	 
	
	printf("\n\n\n\n==MAIN==\n");
	lcd_print ("==MAIN==");

	rtosInit();

	set_cursor (0, 1);
	lcd_print ("==DONE INIT==");
    
	threadObjectCreate(
		&thread1,
		(void *)function1,
		0,
		0,
		0,
		0,
		&stack1[1000],
		1,
		INITIAL_CPSR_ARM_FUNCTION,
		"thread1"
	);
                        
	threadObjectCreate(
		&thread2,
		(void *)function2,
		0,
		0,
		0,
		0,
		&stack2[1000],
		1,
		INITIAL_CPSR_ARM_FUNCTION,
		"thread2"
	);
                        
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
                        
                        
void function1(void)
{
	unsigned int count;
	while(1) {
		LED_Out(count++);
	}
}
                    
void function2(void)
{
	unsigned int count;
	while(1) {
		printf("%d\n",count++);
	}
}
                    
                                
void irq_interrupt_service_routine(void)
{
    timerTick();
    T0IR        = 1;                      /* Clear interrupt flag               */
    VICVectAddr = 0;                      /* Acknowledge Interrupt              */
    return;
}
