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
#include "Config/LoraWAN_Config.h"

SemaphoreHandle_t mutex;
MessageBufferHandle_t downLinkMessageBufferHandle;
sensorData_t sensorData;

void taskTest(void *pvParameters) {
	TickType_t xLastWakeTime = xTaskGetTickCount();
	const TickType_t xFrequency = pdMS_TO_TICKS(20000UL); // 300000 ms = 5 min
	printf("TEST");

  lora_driver_resetRn2483(1);
	vTaskDelay(2);
	lora_driver_resetRn2483(0);
	// Give it a chance to wakeup
	vTaskDelay(150);

	lora_driver_flushBuffers(); // get rid of first version string from module after reset!

	for (;;) {
		xTaskDelayUntil( &xLastWakeTime, xFrequency );
		
		lora_driver_payload_t downlinkPayload;
		
		printf("Receiving downlink...\n");
		if (xMessageBufferReceive(downLinkMessageBufferHandle, &downlinkPayload, sizeof(lora_driver_payload_t), portMAX_DELAY) == 0) {
      printf("Downlink failed...\n");
    }
    else {
      printf("DOWN LINK: from port: %d with %d bytes received!\n", downlinkPayload.portNo, downlinkPayload.len); // Just for Debug
      char testString[5];
      for (int i = 0; i < 4; i++) {
        testString[i] = downlinkPayload.bytes[i];
      }
	  testString[4] = '\0';
      printf("Message from downlink: %s\n", testString);
	  printf("DONE\n");
	  }
  }
}

void createTasks(void) {
	temperatureTransmitParams_t temperatureTransmitParams = temperatureTransmit_createParams(mutex, sensorData);
	temperatureTransmit_createTask(4, (void*)temperatureTransmitParams);
	//cloudDownlinkParams_t cloudDownlinkParams = cloudDownlink_createParams(mutex, downLinkMessageBufferHandle);
	//cloudDownlink_createTask(3, (void*)cloudDownlinkParams);
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
  xTaskCreate(
	taskTest
	,  "Test Task"
	,  configMINIMAL_STACK_SIZE+100
	,  NULL
	,  3  // Priority, with configMAX_PRIORITIES - 1 being the highest, and 0 being the lowest.
	,  NULL );
	printf("\nSystem initialized\nStarting Task Scheduler\n");
	vTaskStartScheduler();
}

