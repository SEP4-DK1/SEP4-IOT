#include "TemperatureTransmit.h"
#include <ATMEGA_FreeRTOS.h>
#include <task.h>

void temperatureTransmit_initialize(void) {
	
}

void temperatureTransmit_createTask(void) {
		xTaskCreate(
		temperatureTransmit_task
		,  "Temperature Transmit Task"
		,  configMINIMAL_STACK_SIZE
		,  NULL
		,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		,  NULL );
}

void temperatureTransmit_task(void *pvParameters) {
	TickType_t xLastWakeTime = xTaskGetTickCount();
	const TickType_t xFrequency = pdMS_TO_TICKS(300000); // 300000 ms = 5 min

	for(;;)
	{
		xTaskDelayUntil( &xLastWakeTime, xFrequency );
		printf("Test!\n");
	}
}