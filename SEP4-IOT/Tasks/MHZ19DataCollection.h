#pragma once
#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>
#include "../DataModels/SensorData.h"

typedef struct mhz19DataCollectionParams {
  SemaphoreHandle_t sensorDataMutex;
  sensorData_t sensorData;
} * mhz19DataCollectionParams_t;

void mhz19DataCollection_taskInit(void* pvParameters);
void mhz19DataCollection_taskRun(void);

mhz19DataCollectionParams_t mhz19DataCollection_createParams(SemaphoreHandle_t sensorDataMutex, sensorData_t sensorData);
void mhz19DataCollection_destroyParams(mhz19DataCollectionParams_t mhz19DataCollectionParams);
void mhz19DataCollection_createTask(UBaseType_t taskPriority, void* pvParameters);
void mhz19DataCollection_task(void* pvParameters);