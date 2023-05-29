#include "HIH8120DataCollection.h"
#include <hih8120.h>
#include <stdio.h>
#include <stdlib.h>

#include "../Util/MutexDefinitions.h"

hih8120DataCollectionParams_t hih8120DataCollection_createParams(SemaphoreHandle_t sensorDataMutex, sensorData_t sensorData) {
  hih8120DataCollectionParams_t hih8120DataCollectionParams;
  hih8120DataCollectionParams = malloc(sizeof(*hih8120DataCollectionParams));
  hih8120DataCollectionParams->sensorDataMutex = sensorDataMutex;
  hih8120DataCollectionParams->sensorData = sensorData;
  return hih8120DataCollectionParams;
}

void hih8120DataCollection_destroyParams(hih8120DataCollectionParams_t hih8120DataCollectionParams) {
  if (hih8120DataCollectionParams != NULL) {
    free(hih8120DataCollectionParams);
  }
}

void hih8120DataCollection_createTask(UBaseType_t taskPriority, void* pvParameters) {
  xTaskCreate(
  hih8120DataCollection_task
  ,  "Hih8120 Data Collection Task"
  ,  configMINIMAL_STACK_SIZE+50
  ,  pvParameters
  ,  taskPriority  // Priority, with configMAX_PRIORITIES - 1 being the highest, and 0 being the lowest.
  ,  NULL );
}

SemaphoreHandle_t hih8120DataCollection_sensorDataMutex;
sensorData_t hih8120DataCollection_sensorData;

inline void hih8120DataCollection_initTask(void* pvParameters) {
  hih8120DataCollectionParams_t params = (hih8120DataCollectionParams_t)pvParameters;
  hih8120DataCollection_sensorDataMutex = params->sensorDataMutex;
  hih8120DataCollection_sensorData = params->sensorData;
  hih8120DataCollection_destroyParams(params);
  
  // Measure to get rid of bad temperature value
  if (hih8120_wakeup() != HIH8120_OK) {
    printf("ERROR: Wakeup HIH8120 failed\n");
  }
  vTaskDelay(pdMS_TO_TICKS(50UL));
  if (hih8120_measure() != HIH8120_OK) {
    printf("ERROR: Measure HIH8120 failed\n");
  }
  vTaskDelay(pdMS_TO_TICKS(1UL));
}

inline void hih8120DataCollection_taskRun(void) {
  if (xSemaphoreTake(hih8120DataCollection_sensorDataMutex, pdMS_TO_TICKS(MUTEXBLOCKTIMEMS)) == pdTRUE) {
    sensorData_hih8120Measure(hih8120DataCollection_sensorData);
    xSemaphoreGive(hih8120DataCollection_sensorDataMutex);
  }
}

void hih8120DataCollection_task(void* pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(10000UL); // 10000ms = 10s

  hih8120DataCollection_initTask(pvParameters);

  for (;;) {
    xTaskDelayUntil( &xLastWakeTime, xFrequency );
    hih8120DataCollection_taskRun();
  }
}