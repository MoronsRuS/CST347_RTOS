#include <LPC23xx.H>                    /* LPC23xx definitions                */

#include <stdio.h>
#include <stdlib.h>
#include <rtos.h>

#include <lcd.h>

semaphoreObject_t   testSemaphore;

threadObject_t thread1, thread2, thread3;
void function1(void);
void function2(void);
void function3(void);
int32 stack1[1000], stack2[1000], stack3[1000];

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
	lcd_print ("  SROS DEMO  ");
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
		2,
		INITIAL_CPSR_ARM_FUNCTION,
		"thread2"
	);
                        
	threadObjectCreate(
		&thread3,
		(void *)function3,
		0,
		0,
		0,
		0,
		&stack3[1000],
		3,
		INITIAL_CPSR_ARM_FUNCTION,
		"thread3"
	);
                        
	semaphoreObjectInit(&testSemaphore, 0);
    
	srand(1);
                        
	scheduler();            //This function will never return.
}                       
                        
                        
void function1(void)
{
	int choice;
	int waitTime;
	
	while(1)
	{
		choice = rand() % 2 + 1;
		
		waitTime = rand() % 3 - 1;

		switch(choice) {
		case 1:
			printf("trying to pend with testSamaphore in thread1 with waitTime %d\n", waitTime);
			if(semaphoreObjectPend(&testSemaphore, waitTime)) {
				printf("testSamaphore pend successful in thread1 with waitTime %d\n", waitTime);
			} else {
				printf("testSamaphore pend failed in thread1 with waitTime %d\n", waitTime);
			}
			break;
            
		case 2:
			printf("trying to post testSemaphore in thread1\n");
			semaphoreObjectPost(&testSemaphore);
			printf("testSemaphore got posted in thread1\n");
			break;
		}
	}
}
                    
void function2(void)
{
	int choice;
	int waitTime;

	while(1) {
		choice = rand() % 2 + 1;
		waitTime = rand() % 3 - 1;

		switch(choice) {
		case 1:
			printf("trying to pend with testSamaphore in thread2 with waitTime %d\n", waitTime);
			if(semaphoreObjectPend(&testSemaphore, waitTime)) {
				printf("testSamaphore pend successful in thread2 with waitTime %d\n", waitTime);
			} else {
				printf("testSamaphore pend failed in thread2 with waitTime %d\n", waitTime);
			}
		break;

		case 2:
			printf("trying to post testSemaphore in thread2\n");
			semaphoreObjectPost(&testSemaphore);
			printf("testSemaphore got posted in thread2\n");
		break;
		}
	}
}
                    
                                
void function3(void)
{
	while(1) {
		printf("trying to post testSemaphore in thread3\n");
		semaphoreObjectPost(&testSemaphore);
		printf("testSemaphore got posted in thread3\n");
	}
}


