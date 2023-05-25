#include "ClimateControl.h"
#include "../Util/MutexDefinitions.h"
#include "rc_servo.h"
#include <stdio.h>
#include <stdlib.h>

climateControlParams_t climateControl_createParams(SemaphoreHandle_t sensorDataMutex, SemaphoreHandle_t breadConfigMutex, sensorData_t sensorData, breadConfig_t breadConfig) {
  climateControlParams_t climateControlParams;
  climateControlParams = malloc(sizeof(*climateControlParams));
  climateControlParams->sensorDataMutex = sensorDataMutex;
  climateControlParams->breadConfigMutex = breadConfigMutex;
  climateControlParams->sensorData = sensorData;
  climateControlParams->breadConfig = breadConfig;
  return climateControlParams;
}

void climateControl_destroyParams(climateControlParams_t climateControlParams) {
  if (climateControlParams != NULL) {
    free(climateControlParams);
  }
}

void climateControl_createTask(UBaseType_t taskPriority, void* pvParameters) {
  xTaskCreate(
  climateControl_task
  ,  "Climate Control Task"
  ,  configMINIMAL_STACK_SIZE+200
  ,  pvParameters
  ,  taskPriority  // Priority, with configMAX_PRIORITIES - 1 being the highest, and 0 being the lowest.
  ,  NULL );
}

SemaphoreHandle_t climateControl_sensorDataMutex;
SemaphoreHandle_t climateControl_breadConfigMutex;
sensorData_t climateControl_sensorData;
breadConfig_t climateControl_breadConfig;

inline void climateControl_taskInit(void* pvParameters) {
  climateControlParams_t params = (climateControlParams_t)pvParameters;
  climateControl_sensorDataMutex = params->sensorDataMutex;
  climateControl_breadConfigMutex = params->breadConfigMutex;
  climateControl_sensorData = params->sensorData;
  climateControl_breadConfig = params->breadConfig;
  climateControl_destroyParams(params);
}

inline void climateControl_taskRun(void) {
  if (xSemaphoreTake(climateControl_sensorDataMutex, pdMS_TO_TICKS(MUTEXBLOCKTIMEMS)) == pdTRUE 
      && xSemaphoreTake(climateControl_breadConfigMutex, pdMS_TO_TICKS(MUTEXBLOCKTIMEMS)) == pdTRUE) {


    if (climateControl_breadConfig->temperature == 0 && climateControl_breadConfig->humidity == 0) {
      rc_servo_setPosition(SERVO0, HEATEROFF);
      rc_servo_setPosition(SERVO1, VENTILATIONCLOSE);
      
      xSemaphoreGive(climateControl_sensorDataMutex);
      xSemaphoreGive(climateControl_breadConfigMutex);
      return;
    }

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
        || climateControl_sensorData->latestCarbondioxide >= CO2LIMIT) {
      // Open ventilation
      rc_servo_setPosition(SERVO1, VENTILATIONOPEN);
    }
    else {
      // Close ventilation
      rc_servo_setPosition(SERVO1, VENTILATIONCLOSE);
    }
    
    
    if (climateControl_sensorData->latestHumidity < climateControl_breadConfig->humidity - 10) {
      // Turn up heater 100% for 3 sec
      rc_servo_setPosition(SERVO0, HEATER100PERCENT);
      vTaskDelay(pdMS_TO_TICKS(3000L));

      rc_servo_setPosition(SERVO0, HEATER12PERCENT);
    }

    xSemaphoreGive(climateControl_sensorDataMutex);
    xSemaphoreGive(climateControl_breadConfigMutex);
  };
}

void climateControl_task(void* pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(2000UL); // 2000ms = 2s
  climateControl_taskInit(pvParameters);

  for (;;)
  {
    xTaskDelayUntil( &xLastWakeTime, xFrequency );
    climateControl_taskRun();
  }
}