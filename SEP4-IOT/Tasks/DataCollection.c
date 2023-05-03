#include "DataCollection.h"
#include "../SensorData.h"

void dataCollection_createTask(UBaseType_t taskPriority){
		xTaskCreate(
		dataCollection_task
		,  "Data Collection Task"
		,  configMINIMAL_STACK_SIZE+200
		,  NULL
		,  taskPriority  // Priority, with configMAX_PRIORITIES - 1 being the highest, and 0 being the lowest.
		,  NULL );
}

void dataCollection_task(void *pvParameters){
  TickType_t xLastWakeTime = xTaskGetTickCount();
	const TickType_t xFrequency = pdMS_TO_TICKS(10000UL); // 10000ms = 10s

  sensorData_reset();

    for (;;)
    {
		xTaskDelayUntil( &xLastWakeTime, xFrequency );
        sensorData_measure();
    }
};