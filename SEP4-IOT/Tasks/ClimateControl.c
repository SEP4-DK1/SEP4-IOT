#include "ClimateControl.h"
#include <hih8120.h>
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

void climateControl_task(void *pvParameters){
	TickType_t xLastWakeTime = xTaskGetTickCount();
	const TickType_t xFrequency = pdMS_TO_TICKS(2000UL); // 2000ms = 2s
	
	climateControlParams_t params = (climateControlParams_t)pvParameters;
	SemaphoreHandle_t mutex = params->mutex;
	sensorData_t sensorData = params->sensorData;
	breadConfig_t breadConfig = params->breadConfig;
	climateControl_destroyParams(params);

	for (;;)
	{
		xTaskDelayUntil( &xLastWakeTime, xFrequency );

    if (sensorData->latestTemperature = breadConfig->temperature) {
      // Turn up heater to 5% as a holding effect
    }
	 else if (sensorData->latestTemperature < breadConfig->temperature - 300) {
      // Turn up heater to 80%
    }
	 else if (sensorData->latestTemperature < breadConfig->temperature - 200) {
      // Turn up heater to 60%
    }
    else if (sensorData->latestTemperature < breadConfig->temperature - 100) {
      // Turn up heater to 50%
    }
    else if (sensorData->latestTemperature < breadConfig->temperature - 50) {
      // Turn up heater to 10%
    }

    if (sensorData->latestTemperature > breadConfig->temperature + 10) {
      // turn heater to 0% 
    }

	if (sensorData->latestTemperature > breadConfig->temperature + 50) {
      //open ventilation
    }

	/*
	if (sensorData->latestHumidity > breadConfig->humidity + 5) {
      // open ventilation 
    }

	else if (sensorData->latestHumidity = breadConfig->humidity){
		// Turn up heater to 5% as a holding effect
	}

	else if (sensorData->latestHumidity < breadConfig->humidity - 5){
		// Turn up heater 100% for 3 sec
	}

	if (sensorData->latestCo2 =< 2500) {
      // we will set 2500ppm as our maxvalue, 
	  // https://www.canada.ca/en/health-canada/programs/consultation-residential-indoor-air-quality-guidelines-carbon-dioxide/document.html#:~:text=of%20this%20assessment.-,Figure%201.%20Comparison%20of%20CO2%20concentrations%20in%20Canadian%20homes%20to%20CO2%20concentrations%20associated%20with%20health,-effects
	  // in this link tells us what happens at the difrent consentrations of Co2 We agreed to keep it safe to the baker opening it. 
	  // open ventilation 
    }

	*/

	}
}