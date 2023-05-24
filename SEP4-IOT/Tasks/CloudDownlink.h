#pragma once
#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>
#include <lora_driver.h>
#include "../DataModels/BreadConfig.h"

typedef struct cloudDownlinkParams {
	SemaphoreHandle_t mutex;
  MessageBufferHandle_t messageBufferHandle;
  breadConfig_t breadConfig;
} * cloudDownlinkParams_t;

void cloudDownlink_taskInit(void* pvParameters);
void cloudDownlink_taskRun();

cloudDownlinkParams_t cloudDownlink_createParams(SemaphoreHandle_t mutex, MessageBufferHandle_t messageBufferHandle, breadConfig_t breadConfig);
void cloudDownlink_destroyParams(cloudDownlinkParams_t cloudDownlinkParams);
void cloudDownlink_createTask(UBaseType_t taskPriority, void* pvParameters);
void cloudDownlink_task(void *pvParameters);