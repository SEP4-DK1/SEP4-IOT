#pragma once
#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>
#include <lora_driver.h>
#include "../DataModels/SensorData.h"

typedef struct temperatureTransmitParams {
	SemaphoreHandle_t mutex;
	sensorData_t sensorData;
	MessageBufferHandle_t messageBufferHandle;
} * temperatureTransmitParams_t;

temperatureTransmitParams_t temperatureTransmit_createParams(SemaphoreHandle_t mutex, sensorData_t sensorData, MessageBufferHandle_t messageBufferHandle);
void temperatureTransmit_destroyParams(temperatureTransmitParams_t temperatureTransmitParams);
void temperatureTransmit_createTask(UBaseType_t taskPriority, void* pvParameters);
void temperatureTransmit_task(void *pvParameters);