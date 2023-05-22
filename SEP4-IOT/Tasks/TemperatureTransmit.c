#include "TemperatureTransmit.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include "../Util/LoRaWANUtil.h"

#include <lora_driver.h>

#include "../DataModels/SensorData.h"

temperatureTransmitParams_t temperatureTransmit_createParams(SemaphoreHandle_t mutex, sensorData_t sensorData) {
	temperatureTransmitParams_t temperatureTransmitParams;
	temperatureTransmitParams = malloc(sizeof(*temperatureTransmitParams));
	temperatureTransmitParams->mutex = mutex;
	temperatureTransmitParams->sensorData = sensorData;
	return temperatureTransmitParams;
}

void temperatureTransmit_destroyParams(temperatureTransmitParams_t temperatureTransmitParams) {
	if (temperatureTransmitParams != NULL) {
		free(temperatureTransmitParams);
	}
}

void temperatureTransmit_createTask(UBaseType_t taskPriority, void* pvParameters) {
		xTaskCreate(
		temperatureTransmit_task
		,  "Temperature Transmit Task"
		,  configMINIMAL_STACK_SIZE+200
		,  pvParameters
		,  taskPriority  // Priority, with configMAX_PRIORITIES - 1 being the highest, and 0 being the lowest.
		,  NULL );
}

SemaphoreHandle_t mutex;
lora_driver_payload_t _uplink_payload;
sensorData_t sensorData;

inline void temperatureTransmit_taskInit(void* pvParameters) {
	LoRaWANUtil_setup();

  temperatureTransmitParams_t params = (temperatureTransmitParams_t) pvParameters;
	mutex = params->mutex;
	sensorData = params->sensorData;
	temperatureTransmit_destroyParams(params);
  
	sensorData = (sensorData_t) pvParameters;

	_uplink_payload.len = 4;
	_uplink_payload.portNo = 1;
}

inline void temperatureTransmit_taskRun() {
  uint16_t temperature = sensorData_getTemperatureAverage(sensorData);
	uint16_t humidity = sensorData_getHumidityAverage(sensorData);
	uint16_t carbondioxid = sensorData_getCarbondioxideAverage(sensorData);
	sensorData_reset(sensorData);
  
	printf("Average Temperature: %d\n", temperature);
	printf("Average Humidity: %d\n", humidity);
	printf("Average Co2: %d\n", carbondioxid);

	// Clear payload bytes
	for (uint8_t i = 0; i < _uplink_payload.len; i++) {
		_uplink_payload.bytes[i] = 0;
	}
	
	_uplink_payload.bytes[0] = (char) temperature;
	_uplink_payload.bytes[1] |= ((char) (temperature >> 2)) & 0b11000000;
  
	_uplink_payload.bytes[1] |= (char) (humidity & 0b00111111);
	_uplink_payload.bytes[2] |= ((char) (humidity << 1)) & 0b10000000;

	_uplink_payload.bytes[2] |= (char) (carbondioxid & 0b01111111);
	_uplink_payload.bytes[3] |= (char) ((carbondioxid >> 6) & 0b11111100);
	
  LoRaWANUtil_sendPayload(&_uplink_payload);
}

void temperatureTransmit_task(void* pvParameters) {
	TickType_t xLastWakeTime = xTaskGetTickCount();
	const TickType_t xFrequency = pdMS_TO_TICKS(300000UL); // 300000 ms = 5 min
	
	temperatureTransmit_taskInit(pvParameters);


	for(;;)
	{
		xTaskDelayUntil( &xLastWakeTime, xFrequency );
		temperatureTransmit_taskRun();
	}
}