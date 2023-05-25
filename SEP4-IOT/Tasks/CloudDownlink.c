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
#include "../Util/MutexDefinitions.h"

cloudDownlinkParams_t cloudDownlink_createParams(SemaphoreHandle_t breadConfigMutex, MessageBufferHandle_t messageBufferHandle, breadConfig_t breadConfig) {
  cloudDownlinkParams_t cloudDownlinkParams;
  cloudDownlinkParams = malloc(sizeof(*cloudDownlinkParams));
  cloudDownlinkParams->breadConfigMutex = breadConfigMutex;
  cloudDownlinkParams->messageBufferHandle = messageBufferHandle;
  cloudDownlinkParams->breadConfig = breadConfig;

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
  ,  configMINIMAL_STACK_SIZE+100
  ,  pvParameters
  ,  taskPriority  // Priority, with configMAX_PRIORITIES - 1 being the highest, and 0 being the lowest.
  ,  NULL );
}

SemaphoreHandle_t cloudDownlink_breadConfigMutex;
MessageBufferHandle_t downLinkMessageBufferHandle;
breadConfig_t cloudDownlink_breadConfig;

inline void cloudDownlink_taskInit(void* pvParameters) {
  cloudDownlinkParams_t params = (cloudDownlinkParams_t) pvParameters;
  cloudDownlink_breadConfigMutex = params->breadConfigMutex;
  downLinkMessageBufferHandle = params->messageBufferHandle;
  cloudDownlink_breadConfig = params->breadConfig;
  cloudDownlink_destroyParams(params);
}

inline void cloudDownlink_taskRun() {
  lora_driver_payload_t downlinkPayload;

  printf("Listening for downlink...\n");
  if (xMessageBufferReceive(downLinkMessageBufferHandle, &downlinkPayload, sizeof(lora_driver_payload_t), portMAX_DELAY) == 0) {
    printf("Downlink failed (payload was empty)\n");
  }
  else {
    printf("DOWN LINK: from port: %d with %d bytes received!\n", downlinkPayload.portNo, downlinkPayload.len); // Just for Debug
    if (downlinkPayload.len == 3) {
      uint16_t readTemp = (uint16_t) (((downlinkPayload.bytes[1] & 0b11000000) << 2) + (0b11111111 & downlinkPayload.bytes[0]));
      uint8_t readHum = (uint8_t) (((downlinkPayload.bytes[2] & 0b10000000) >> 1) + (downlinkPayload.bytes[1] & 0b00111111));

      if (xSemaphoreTake(cloudDownlink_breadConfigMutex, pdMS_TO_TICKS(MUTEXBLOCKTIMEMS)) == pdTRUE) {
        cloudDownlink_breadConfig->temperature = readTemp;
        cloudDownlink_breadConfig->humidity = readHum;
        xSemaphoreGive(cloudDownlink_breadConfigMutex);
      }
      printf("Temperature received: %d\tHumidity received: %d\n", readTemp, readHum);
    }
  }
}

void cloudDownlink_task(void* pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(240000UL); // This delay doesn't really matter as xMessageBufferReceive will pause this task until an uplink message is sent from the temperatureTransmit task
  
  cloudDownlink_taskInit(pvParameters);

  for(;;) {
    xTaskDelayUntil( &xLastWakeTime, xFrequency );
    cloudDownlink_taskRun();		
  }
}