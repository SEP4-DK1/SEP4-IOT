#include <stdio.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>

#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <stdio_driver.h>
#include <serial.h>

#include "Tasks/TemperatureTransmit.h"

SemaphoreHandle_t mutex;

void createTasks(void) {
	temperatureTransmit_createTask();
}

void initTasks(void) {
	temperatureTransmit_initialize();
}

void initialiseSystem(void) {
	mutex = xSemaphoreCreateMutex(); // Init mutex
	
	// Make it possible to use stdio on COM port 0 (USB) on Arduino board - Setting 57600,8,N,1
	stdio_initialise(ser_USART0);

	createTasks();
	initTasks();
}

int main(void) {
	initialiseSystem();
	printf("System initialized\nStarting Task Scheduler\n");
	vTaskStartScheduler(); 
}

