#include <stdio.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>

#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <stdio_driver.h>
#include <serial.h>
#include <lora_driver.h>
#include <status_leds.h>

#include "Tasks/TemperatureTransmit.h"

SemaphoreHandle_t mutex;

void createTasks(void) {
	temperatureTransmit_createTask(3);
}

void runTaskSetups(void) {
	// runTaskSetups function might not be useful, but it's kept for now
}

void initialiseSystem(void) {
	mutex = xSemaphoreCreateMutex(); // Init mutex
	
	// Make it possible to use stdio on COM port 0 (USB) on Arduino board - Setting 57600,8,N,1
	stdio_initialise(ser_USART0);
	
	// Not sure if this code should be moved somewhere else...
	DDRA |= _BV(DDA0) | _BV(DDA7);
	status_leds_initialise(5); // Priority 5 for internal task
	lora_driver_initialise(ser_USART1, NULL);
	
	runTaskSetups();
	createTasks();
}

int main(void) {
	initialiseSystem();
	printf("\nSystem initialized\nStarting Task Scheduler\n");
	vTaskStartScheduler(); 
}

