#pragma once
#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>
#include "../DataModels/SensorData.h"

typedef struct cloudUplinkParams {
  SemaphoreHandle_t sensorDataMutex;
  sensorData_t sensorData;
} * cloudUplinkParams_t;

void cloudUplink_taskInit(void* pvParameters);
void cloudUplink_taskRun();

cloudUplinkParams_t cloudUplink_createParams(SemaphoreHandle_t sensorDataMutex, sensorData_t sensorData);
void cloudUplink_destroyParams(cloudUplinkParams_t cloudUplinkParams);
void cloudUplink_createTask(UBaseType_t taskPriority, void* pvParameters);
void cloudUplink_task(void *pvParameters);