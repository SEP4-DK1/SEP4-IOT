#pragma once
#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>
#include "../DataModels/SensorData.h"
#include "../DataModels/BreadConfig.h"

typedef struct climateControlParams {
	SemaphoreHandle_t mutex;
	sensorData_t sensorData;
    breadConfig_t breadConfig;
} * climateControlParams_t;

climateControlParams_t climateControl_createParams(SemaphoreHandle_t mutex, sensorData_t sensorData, breadConfig_t breadConfig);
void climateControl_destroyParams(climateControlParams_t dataCollectionParams);
void climateControl_createTask(UBaseType_t taskPriority, void* pvParameters);
void climateControl_task(void *pvParameters);