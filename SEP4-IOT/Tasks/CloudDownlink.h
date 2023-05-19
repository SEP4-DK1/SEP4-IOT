#pragma once
#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>
#include <lora_driver.h>

typedef struct cloudDownlinkParams {
	SemaphoreHandle_t mutex;
  MessageBufferHandle_t messageBufferHandle;
} * cloudDownlinkParams_t;

cloudDownlinkParams_t cloudDownlink_createParams(SemaphoreHandle_t mutex, MessageBufferHandle_t messageBufferHandle);
void cloudDownlink_destroyParams(cloudDownlinkParams_t cloudDownlinkParams);
void cloudDownlink_createTask(UBaseType_t taskPriority, void* pvParameters);
void cloudDownlink_task(void *pvParameters);