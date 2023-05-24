
#pragma once
#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>
#include "../DataModels/SensorData.h"
#include "../DataModels/BreadConfig.h"

#define SERVO0 0
#define SERVO1 1
#define HEATEROFF -100
#define HEATER12PERCENT -25
#define HEATER25PERCENT -50
#define HEATER50PERCENT 0
#define HEATER75PERCENT 50
#define HEATER100PERCENT 100

#define VENTILATIONOPEN 100
#define VENTILATIONCLOSE -100

typedef struct climateControlParams {
	SemaphoreHandle_t mutex;
	sensorData_t sensorData;
    breadConfig_t breadConfig;
} * climateControlParams_t;

void climateControl_taskRun();
void climateControl_taskInit(void *pvParameters);

climateControlParams_t climateControl_createParams(SemaphoreHandle_t mutex, sensorData_t sensorData, breadConfig_t breadConfig);
void climateControl_destroyParams(climateControlParams_t dataCollectionParams);
void climateControl_createTask(UBaseType_t taskPriority, void* pvParameters);
void climateControl_task(void *pvParameters);