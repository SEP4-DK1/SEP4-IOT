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
#include <mh_z19.h>

#include "DataModels/SensorData.h"
#include "DataModels/BreadConfig.h"
#include "Tasks/TemperatureTransmit.h"
#include "Tasks/CloudDownlink.h"
#include "Tasks/DataCollection.h"
#include "Tasks/ClimateControl.h"

SemaphoreHandle_t mutex;
MessageBufferHandle_t downLinkMessageBufferHandle;
sensorData_t sensorData;
breadConfig_t breadConfig;

void createTasks(void) {
	temperatureTransmitParams_t temperatureTransmitParams = temperatureTransmit_createParams(mutex, sensorData);
	temperatureTransmit_createTask(5, (void*)temperatureTransmitParams);
	cloudDownlinkParams_t cloudDownlinkParams = cloudDownlink_createParams(mutex, downLinkMessageBufferHandle, breadConfig);
	cloudDownlink_createTask(4, (void*)cloudDownlinkParams);
	dataCollectionParams_t dataCollectionParams = dataCollection_createParams(mutex, sensorData);
	dataCollection_createTask(3, (void*)dataCollectionParams);
	climateControlParams_t climateControlParams = climateControl_createParams(mutex, sensorData, breadConfig);
	climateControl_createTask(2, (void*)climateControlParams);
}

void initialiseSystem(void) {
	mutex = xSemaphoreCreateMutex(); // Init mutex
	
	// Driver initialisation
	stdio_initialise(ser_USART0); // Make it possible to use stdio on COM port 0 (USB) on Arduino board - Setting 57600,8,N,1
	status_leds_initialise(5); // Priority 5 for internal task
	hih8120_initialise();
	mh_z19_initialise(ser_USART3); // This port is taken from the documentation (https://ihavn.github.io/IoT_Semester_project/mh_z19_driver_quick_start.html#mh_z19_use_cases)
	rc_servo_initialise();
	downLinkMessageBufferHandle = xMessageBufferCreate(sizeof(lora_driver_payload_t) * 2);
	lora_driver_initialise(ser_USART1, downLinkMessageBufferHandle);

	sensorData = sensorData_init();
	breadConfig = breadConfig_init();
	createTasks();
}

int main(void) {
	initialiseSystem();
	printf("\nSystem initialized\nStarting Task Scheduler\n");
	vTaskStartScheduler();
}

