#pragma once
#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>
#include "../DataModels/SensorData.h"

typedef struct hih8120DataCollectionParams {
  SemaphoreHandle_t sensorDataMutex;
  sensorData_t sensorData;
} * hih8120DataCollectionParams_t;

void hih8120DataCollection_taskInit(void* pvParameters);
void hih8120DataCollection_taskRun(void);

hih8120DataCollectionParams_t hih8120DataCollection_createParams(SemaphoreHandle_t sensorDataMutex, sensorData_t sensorData);
void hih8120DataCollection_destroyParams(hih8120DataCollectionParams_t dataCollectionParams);
void hih8120DataCollection_createTask(UBaseType_t taskPriority, void* pvParameters);
void hih8120DataCollection_task(void* pvParameters);