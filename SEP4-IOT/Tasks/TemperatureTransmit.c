#include "TemperatureTransmit.h"
#include <task.h>
#include <stdio.h>
#include "../Util/LoRaWANUtil.h"

#include <lora_driver.h>

#include "../DataModels/SensorData.h"

void temperatureTransmit_createTask(UBaseType_t taskPriority, void* pvParameters) {
		xTaskCreate(
		temperatureTransmit_task
		,  "Temperature Transmit Task"
		,  configMINIMAL_STACK_SIZE+100
		,  pvParameters
		,  taskPriority  // Priority, with configMAX_PRIORITIES - 1 being the highest, and 0 being the lowest.
		,  NULL );
}

// Test Helper Function (Move this somewhere else at some point!)
void printBits(size_t const size, void const * const ptr)
{
    unsigned char const *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    
    for (i = 0; i < size; i++) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
		printf(" ");
    }
    puts("");
}

lora_driver_payload_t _uplink_payload;
sensorData_t sensorData;

inline void temperatureTransmit_taskInit(void* pvParameters) {
	LoRaWANUtil_setup();

	sensorData = (sensorData_t) pvParameters;

	_uplink_payload.len = 2;
	_uplink_payload.portNo = 1;
}

inline void temperatureTransmit_taskRun() {
	uint16_t temperature = sensorData_getTemperatureAverage(sensorData);
	sensorData_reset(sensorData);
	printf("Average Minute Temperature: %d\n", temperature);

	// Clear payload bytes
	for (uint8_t i = 0; i < _uplink_payload.len; i++) {
		_uplink_payload.bytes[i] = 0;
	}
	
	_uplink_payload.bytes[0] = (char) temperature;
	_uplink_payload.bytes[1] |= ((char) (temperature >> 2)) & 0b11000000;

	// _uplink_payload.bytes[1] |= ((char) (humidity >> 6)) & 0b00111111;
	// _uplink_payload.bytes[2] |= ((char) (humidity >> 1)) & 0b10000000;
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