#pragma once
#include <ATMEGA_FreeRTOS.h>

void dataCollection_createTask(UBaseType_t taskPriority, void* pvParameters);
void dataCollection_task(void *pvParameters);