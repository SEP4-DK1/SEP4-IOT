#include "DataCollection.h"
#include <hih8120.h>
#include <stdio.h>
#include <stdlib.h>

dataCollectionParams_t dataCollection_createParams(SemaphoreHandle_t mutex, sensorData_t sensorData) {
	dataCollectionParams_t dataCollectionParams;
	dataCollectionParams = malloc(sizeof(*dataCollectionParams));
	dataCollectionParams->mutex = mutex;
	dataCollectionParams->sensorData = sensorData;
	return dataCollectionParams;
}
void dataCollection_destroyParams(dataCollectionParams_t dataCollectionParams) {
	if (dataCollectionParams != NULL) {
		free(dataCollectionParams);
	}
}

void dataCollection_createTask(UBaseType_t taskPriority, void* pvParameters){
		xTaskCreate(
		dataCollection_task
		,  "Data Collection Task"
		,  configMINIMAL_STACK_SIZE+200
		,  pvParameters
		,  taskPriority  // Priority, with configMAX_PRIORITIES - 1 being the highest, and 0 being the lowest.
		,  NULL );
}

void dataCollection_task(void *pvParameters){
	TickType_t xLastWakeTime = xTaskGetTickCount();
	const TickType_t xFrequency = pdMS_TO_TICKS(10000UL); // 10000ms = 10s
	
	dataCollectionParams_t params = (dataCollectionParams_t)pvParameters;
	SemaphoreHandle_t mutex = params->mutex;
	sensorData_t sensorData = params->sensorData;
	dataCollection_destroyParams(params);
	
	// Measure to get rid of bad temperature value
	if (hih8120_wakeup() != HIH8120_OK) {
		printf("ERROR: Wakeup HIH8120 failed\n");
	}
	vTaskDelay(pdMS_TO_TICKS(50UL));
	if (hih8120_measure() != HIH8120_OK) {
		printf("ERROR: Measure HIH8120 failed\n");
	}
	vTaskDelay(pdMS_TO_TICKS(1UL));

	for (;;)
	{
		xTaskDelayUntil( &xLastWakeTime, xFrequency );
		sensorData_measure(sensorData);
		
	}
}