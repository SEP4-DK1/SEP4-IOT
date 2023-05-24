#include "ClimateControl.h"
#include "rc_servo.h"
#include <stdio.h>
#include <stdlib.h>

climateControlParams_t climateControl_createParams(SemaphoreHandle_t mutex, sensorData_t sensorData, breadConfig_t breadConfig) {
  climateControlParams_t climateControlParams;
  climateControlParams = malloc(sizeof(*climateControlParams));
  climateControlParams->mutex = mutex;
  climateControlParams->sensorData = sensorData;
  climateControlParams->breadConfig = breadConfig;
  return climateControlParams;
}
void climateControl_destroyParams(climateControlParams_t climateControlParams) {
  if (climateControlParams != NULL) {
    free(climateControlParams);
  }
}

void climateControl_createTask(UBaseType_t taskPriority, void* pvParameters){
    xTaskCreate(
    climateControl_task
    ,  "Climate Control Task"
    ,  configMINIMAL_STACK_SIZE+200
    ,  pvParameters
    ,  taskPriority  // Priority, with configMAX_PRIORITIES - 1 being the highest, and 0 being the lowest.
    ,  NULL );
}
SemaphoreHandle_t climateControl_mutex;
sensorData_t climateControl_sensorData;
breadConfig_t climateControl_breadConfig;

void climateControl_taskInit(void *pvParameters) {
  climateControlParams_t params = (climateControlParams_t)pvParameters;
  climateControl_mutex = params->mutex;
  climateControl_sensorData = params->sensorData;
  climateControl_breadConfig = params->breadConfig;
  climateControl_destroyParams(params);
}

void climateControl_taskRun() {
  if (climateControl_sensorData->latestTemperature > climateControl_breadConfig->temperature + 100) {
    // turn heater to 0% 
    rc_servo_setPosition(SERVO0, HEATEROFF);
  }
  else if (climateControl_breadConfig->temperature - climateControl_sensorData->latestTemperature <= 50) {
    // Turn up heater to 12.5% as a holding effect
    rc_servo_setPosition(SERVO0, HEATER12PERCENT);
  }
  else if (climateControl_breadConfig->temperature - climateControl_sensorData->latestTemperature <= 75) {
    // Turn up heater to 25%
    rc_servo_setPosition(SERVO0, HEATER25PERCENT);
  }
  else if (climateControl_breadConfig->temperature - climateControl_sensorData->latestTemperature <= 100) {
    // Turn up heater to 50%
    rc_servo_setPosition(SERVO0, HEATER50PERCENT);
  }
  else if (climateControl_breadConfig->temperature - climateControl_sensorData->latestTemperature <= 200) {
    // Turn up heater to 75%
    rc_servo_setPosition(SERVO0, HEATER75PERCENT);
  }
  else {
    // Turn up heater to 100%
    rc_servo_setPosition(SERVO0, HEATER100PERCENT);
  }

  if (climateControl_sensorData->latestTemperature > climateControl_breadConfig->temperature + 150
      || climateControl_sensorData->latestHumidity > climateControl_breadConfig->humidity + 5
	    || climateControl_sensorData->latestCarbondioxide >= 2500) {
		// Open ventilation
		rc_servo_setPosition(SERVO1, VENTILATIONOPEN);
	}
	else {
		// Close ventilation
		rc_servo_setPosition(SERVO1, VENTILATIONCLOSE);
	}
  
  
  if (climateControl_sensorData->latestHumidity < climateControl_breadConfig->humidity - 10){
    // Turn up heater 100% for 3 sec
    rc_servo_setPosition(SERVO0, HEATER100PERCENT);
    vTaskDelay(pdMS_TO_TICKS(3000L));

    rc_servo_setPosition(SERVO0, HEATER12PERCENT);
  }
}

void climateControl_task(void *pvParameters){
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(2000UL); // 2000ms = 2s
  climateControl_taskInit(pvParameters);

  for (;;)
  {
    xTaskDelayUntil( &xLastWakeTime, xFrequency );
    climateControl_taskRun();
  }
}