#include <stdio.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>

#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <stdio_driver.h>
#include <serial.h>
#include <lora_driver.h>
#include <status_leds.h>
#include <hih8120.h>
#include <rc_servo.h>
#include <mh_z19.h>

#include "DataModels/SensorData.h"
#include "DataModels/BreadConfig.h"
#include "Tasks/CloudUplink.h"
#include "Tasks/CloudDownlink.h"
#include "Tasks/HIH8120DataCollection.h"
#include "Tasks/MHZ19DataCollection.h"
#include "Tasks/ClimateControl.h"

MessageBufferHandle_t downLinkMessageBufferHandle;
SemaphoreHandle_t sensorDataMutex;
SemaphoreHandle_t breadConfigMutex;
sensorData_t sensorData;
breadConfig_t breadConfig;

void createTasks(void) {
  cloudUplinkParams_t cloudUplinkParams = cloudUplink_createParams(sensorDataMutex, sensorData);
  cloudUplink_createTask(7, (void*) cloudUplinkParams);
  cloudDownlinkParams_t cloudDownlinkParams = cloudDownlink_createParams(breadConfigMutex, downLinkMessageBufferHandle, breadConfig);
  cloudDownlink_createTask(6, (void*) cloudDownlinkParams);
  climateControlParams_t climateControlParams = climateControl_createParams(sensorDataMutex, breadConfigMutex, sensorData, breadConfig);
  climateControl_createTask(3, (void*) climateControlParams);
  hih8120DataCollectionParams_t hih8120DataCollectionParams = hih8120DataCollection_createParams(sensorDataMutex, sensorData);
  hih8120DataCollection_createTask(2, (void*) hih8120DataCollectionParams);
  mhz19DataCollectionParams_t mhz19DataCollectionParams = mhz19DataCollection_createParams(sensorDataMutex, sensorData);
  mhz19DataCollection_createTask(1, (void*) mhz19DataCollectionParams);
}

void initialiseSystem(void) {
  sensorDataMutex = xSemaphoreCreateMutex();
  breadConfigMutex = xSemaphoreCreateMutex();
  
  // Driver initialisation
  stdio_initialise(ser_USART0); // Make it possible to use stdio on COM port 0 (USB) on Arduino board - Setting 57600,8,N,1
  status_leds_initialise(5); // Priority 5 for internal task
  hih8120_initialise();
  mh_z19_initialise(ser_USART3);
  rc_servo_initialise();
  downLinkMessageBufferHandle = xMessageBufferCreate(sizeof(lora_driver_payload_t) * 2);
  lora_driver_initialise(ser_USART1, downLinkMessageBufferHandle);

  sensorData = sensorData_init();
  breadConfig = breadConfig_init();
  createTasks();
}

int main(void) {
  initialiseSystem();
  printf("\nSystem initialized\nStarting Task Scheduler\n");
  vTaskStartScheduler();
}

