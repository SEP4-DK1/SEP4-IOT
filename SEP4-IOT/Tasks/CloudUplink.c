#include "CloudUplink.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include "../Util/LoRaWANUtil.h"

#include <lora_driver.h>

#include "../DataModels/SensorData.h"
#include "../Util/MutexDefinitions.h"

cloudUplinkParams_t cloudUplink_createParams(SemaphoreHandle_t sensorDataMutex, sensorData_t sensorData) {
  cloudUplinkParams_t cloudUplinkParams;
  cloudUplinkParams = malloc(sizeof(*cloudUplinkParams));
  cloudUplinkParams->sensorDataMutex = sensorDataMutex;
  cloudUplinkParams->sensorData = sensorData;
  return cloudUplinkParams;
}

void cloudUplink_destroyParams(cloudUplinkParams_t cloudUplinkParams) {
  if (cloudUplinkParams != NULL) {
    free(cloudUplinkParams);
  }
}

void cloudUplink_createTask(UBaseType_t taskPriority, void* pvParameters) {
  xTaskCreate(
  cloudUplink_task
  ,  "CloudUplink Task"
  ,  configMINIMAL_STACK_SIZE+200
  ,  pvParameters
  ,  taskPriority  // Priority, with configMAX_PRIORITIES - 1 being the highest, and 0 being the lowest.
  ,  NULL );
}

SemaphoreHandle_t cloudUplink_sensorDataMutex;
lora_driver_payload_t uplinkPayload;
sensorData_t sensorData;

inline void cloudUplink_taskInit(void* pvParameters) {
  LoRaWANUtil_setup();

  cloudUplinkParams_t params = (cloudUplinkParams_t) pvParameters;
  cloudUplink_sensorDataMutex = params->sensorDataMutex;
  sensorData = params->sensorData;
  cloudUplink_destroyParams(params);

  uplinkPayload.len = 4;
  uplinkPayload.portNo = 1;
}

inline void cloudUplink_taskRun(void) {
  if (xSemaphoreTake(cloudUplink_sensorDataMutex, pdMS_TO_TICKS(MUTEXBLOCKTIMEMS)) == pdTRUE) {
    uint16_t temperature = sensorData_getTemperatureAverage(sensorData);
    uint16_t humidity = sensorData_getHumidityAverage(sensorData);
    uint16_t carbondioxid = sensorData_getCarbondioxideAverage(sensorData);
    sensorData_reset(sensorData);
    
    xSemaphoreGive(cloudUplink_sensorDataMutex);

    printf("Average Temperature: %d\n", temperature);
    printf("Average Humidity: %d\n", humidity);
    printf("Average Co2: %d\n", carbondioxid);

    // Clear payload bytes
    for (uint8_t i = 0; i < uplinkPayload.len; i++) {
      uplinkPayload.bytes[i] = 0;
    }
    
    uplinkPayload.bytes[0] = (char) temperature;
    uplinkPayload.bytes[1] |= ((char) (temperature >> 2)) & 0b11000000;
    
    uplinkPayload.bytes[1] |= (char) (humidity & 0b00111111);
    uplinkPayload.bytes[2] |= ((char) (humidity << 1)) & 0b10000000;

    uplinkPayload.bytes[2] |= (char) (carbondioxid & 0b01111111);
    uplinkPayload.bytes[3] |= (char) ((carbondioxid >> 5) & 0b11111100);
    
    LoRaWANUtil_sendPayload(&uplinkPayload);
  }
}

void cloudUplink_task(void* pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(300000UL); // 300000 ms = 5 min

  cloudUplink_taskInit(pvParameters);

  for(;;)
  {
    xTaskDelayUntil( &xLastWakeTime, xFrequency );
    cloudUplink_taskRun();
  }
}