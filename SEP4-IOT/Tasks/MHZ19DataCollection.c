#include "MHZ19DataCollection.h"
#include <mh_z19.h>
#include <stdio.h>
#include <stdlib.h>

mhz19DataCollectionParams_t mhz19DataCollection_createParams(SemaphoreHandle_t mutex, sensorData_t sensorData) {
	mhz19DataCollectionParams_t mhz19DataCollectionParams;
	mhz19DataCollectionParams = malloc(sizeof(*mhz19DataCollectionParams));
	mhz19DataCollectionParams->mutex = mutex;
	mhz19DataCollectionParams->sensorData = sensorData;
	return mhz19DataCollectionParams;
}
void mhz19DataCollection_destroyParams(mhz19DataCollectionParams_t mhz19DataCollectionParams) {
	if (mhz19DataCollectionParams != NULL) {
		free(mhz19DataCollectionParams);
	}
}

void mhz19DataCollection_createTask(UBaseType_t taskPriority, void* pvParameters){
		xTaskCreate(
		mhz19DataCollection_task
		,  "MH-Z19 Data Collection Task"
		,  configMINIMAL_STACK_SIZE+50
		,  pvParameters
		,  taskPriority  // Priority, with configMAX_PRIORITIES - 1 being the highest, and 0 being the lowest.
		,  NULL );
}

void mhz19DataCollection_task(void *pvParameters){
	TickType_t xLastWakeTime = xTaskGetTickCount();
	const TickType_t xFrequency = pdMS_TO_TICKS(10000UL); // 10000ms = 10s
	
	mhz19DataCollectionParams_t params = (mhz19DataCollectionParams_t)pvParameters;
	SemaphoreHandle_t mutex = params->mutex;
	sensorData_t sensorData = params->sensorData;
	mhz19DataCollection_destroyParams(params);
	
	// Measure to get rid of bad temperature value
	if (mh_z19_takeMeassuring() != MHZ19_OK){
		printf("ERROR: Measure mh_z19 failed\n");
	}
	vTaskDelay(pdMS_TO_TICKS(1UL));

	for (;;)
	{
		xTaskDelayUntil( &xLastWakeTime, xFrequency );
		sensorData_mhz19Measure(sensorData);
	}
}