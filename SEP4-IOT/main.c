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
#include <hih8120.h>

#include "DataModels/SensorData.h"
#include "Tasks/TemperatureTransmit.h"
#include "Tasks/DataCollection.h"

SemaphoreHandle_t mutex;
sensorData_t sensorData;

void createTasks(void) {
	temperatureTransmit_createTask(3, (void*)sensorData);
	dataCollection_createTask(2, (void*)sensorData);
}

void runTaskSetups(void) {
	// runTaskSetups function might not be useful, but it's kept for now
}

void initialiseSystem(void) {
	mutex = xSemaphoreCreateMutex(); // Init mutex
	
	// Driver initialisation
	stdio_initialise(ser_USART0); // Make it possible to use stdio on COM port 0 (USB) on Arduino board - Setting 57600,8,N,1
	status_leds_initialise(5); // Priority 5 for internal task
	hih8120_initialise();
	lora_driver_initialise(ser_USART1, NULL);

	sensorData = sensorData_init();
	runTaskSetups();
	createTasks();
}

int main(void) {
	initialiseSystem();
	printf("\nSystem initialized\nStarting Task Scheduler\n");
	vTaskStartScheduler(); 
}

