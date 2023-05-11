#pragma once
#include <ATMEGA_FreeRTOS.h>

void temperatureTransmit_createTask(UBaseType_t taskPriority, void* pvParameters);
void temperatureTransmit_task(void *pvParameters);