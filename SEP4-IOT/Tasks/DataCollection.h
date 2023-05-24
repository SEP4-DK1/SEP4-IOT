#pragma once
#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>
#include "../DataModels/SensorData.h"

typedef struct dataCollectionParams {
	SemaphoreHandle_t mutex;
	sensorData_t sensorData;
} * dataCollectionParams_t;

dataCollectionParams_t dataCollection_createParams(SemaphoreHandle_t mutex, sensorData_t sensorData);
void dataCollection_destroyParams(dataCollectionParams_t dataCollectionParams);
void dataCollection_createTask(UBaseType_t taskPriority, void* pvParameters);
void dataCollection_task(void *pvParameters);