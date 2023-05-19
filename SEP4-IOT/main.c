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
#include "Tasks/CloudDownlink.h"
#include "Tasks/DataCollection.h"

SemaphoreHandle_t mutex;
MessageBufferHandle_t downLinkMessageBufferHandle;
sensorData_t sensorData;

void createTasks(void) {
	temperatureTransmitParams_t temperatureTransmitParams = temperatureTransmit_createParams(mutex, sensorData);
	temperatureTransmit_createTask(4, (void*)temperatureTransmitParams);
	cloudDownlinkParams_t cloudDownlinkParams = cloudDownlink_createParams(mutex, downLinkMessageBufferHandle);
	cloudDownlink_createTask(3, (void*)cloudDownlinkParams);
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
	downLinkMessageBufferHandle = xMessageBufferCreate(sizeof(lora_driver_payload_t) * 2);
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

