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
#include <rc_servo.h>

#include "DataModels/SensorData.h"
#include "Tasks/TemperatureTransmit.h"
#include "Tasks/DataCollection.h"

SemaphoreHandle_t mutex;
MessageBufferHandle_t downLinkMessageBufferHandle;
sensorData_t sensorData;

void createTasks(void) {
	temperatureTransmitParams_t temperatureTransmitParams = temperatureTransmit_createParams(mutex, sensorData, downLinkMessageBufferHandle);
	temperatureTransmit_createTask(3, (void*)temperatureTransmitParams);
	dataCollectionParams_t dataCollectionParams = dataCollection_createParams(mutex, sensorData);
	dataCollection_createTask(2, (void*)dataCollectionParams);
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
	rc_servo_initialise();
	downLinkMessageBufferHandle = xMessageBufferCreate(sizeof(lora_driver_payload_t)*2); // Space for 2 payloads
	lora_driver_initialise(ser_USART1, downLinkMessageBufferHandle);

	sensorData = sensorData_init();
	runTaskSetups();
	createTasks();
}

int main(void) {
	initialiseSystem();
	printf("\nSystem initialized\nStarting Task Scheduler\n");
	vTaskStartScheduler(); 
}

