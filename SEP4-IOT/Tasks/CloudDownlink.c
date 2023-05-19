#include "CloudDownlink.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ATMEGA_FreeRTOS.h>
#include <task.h>

#include <lora_driver.h>
#include <status_leds.h>
#include <hih8120.h>

#include "../DataModels/SensorData.h"

cloudDownlinkParams_t cloudDownlink_createParams(SemaphoreHandle_t mutex, MessageBufferHandle_t messageBufferHandle) {
	cloudDownlinkParams_t cloudDownlinkParams;
	cloudDownlinkParams = malloc(sizeof(*cloudDownlinkParams));
	cloudDownlinkParams->mutex = mutex;
	cloudDownlinkParams->messageBufferHandle = messageBufferHandle;


	return cloudDownlinkParams;
}

void cloudDownlink_destroyParams(cloudDownlinkParams_t cloudDownlinkParams) {
	if (cloudDownlinkParams != NULL) {
		free(cloudDownlinkParams);
	}
}

void cloudDownlink_createTask(UBaseType_t taskPriority, void* pvParameters) {
		xTaskCreate(
		cloudDownlink_task
		,  "Cloud Downlink Task"
		,  configMINIMAL_STACK_SIZE+200
		,  pvParameters
		,  taskPriority  // Priority, with configMAX_PRIORITIES - 1 being the highest, and 0 being the lowest.
		,  NULL );
}

void cloudDownlink_task(void* pvParameters) {
	TickType_t xLastWakeTime = xTaskGetTickCount();
	const TickType_t xFrequency = pdMS_TO_TICKS(30000UL); // 300000 ms = 5 min
	
	cloudDownlinkParams_t params = (cloudDownlinkParams_t) pvParameters;
	SemaphoreHandle_t mutex = params->mutex;
	MessageBufferHandle_t downLinkMessageBufferHandle = params->messageBufferHandle;
	cloudDownlink_destroyParams(params);

	for(;;) {
		xTaskDelayUntil( &xLastWakeTime, xFrequency );
		
		lora_driver_payload_t downlinkPayload;
		
		printf("Receiving downlink...\n");
		if (xMessageBufferReceive(downLinkMessageBufferHandle, &downlinkPayload, sizeof(lora_driver_payload_t), pdMS_TO_TICKS(30000)) == 0) {
      printf("Downlink failed...\n");
    }
    else {
      printf("DOWN LINK: from port: %d with %d bytes received!\n", downlinkPayload.portNo, downlinkPayload.len); // Just for Debug
      char testString[4];
      for (int i = 0; i < 4; i++) {
        testString[i] = downlinkPayload.bytes[i];
      }
      printf("Message from downlink: %s\n", testString);
    }
	}
}