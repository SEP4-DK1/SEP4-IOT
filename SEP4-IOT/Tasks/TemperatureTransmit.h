#pragma once
#include <ATMEGA_FreeRTOS.h>

void temperatureTransmit_createTask(UBaseType_t taskPriority);
void temperatureTransmit_task(void *pvParameters);