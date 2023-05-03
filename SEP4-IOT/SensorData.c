#include "SensorData.h"
#include <ATMEGA_FreeRTOS.h>
#include <hih8120.h>
#include <stdio.h>

static sensorData_t sensorData;

void sensorData_measure(){
    hih8120_wakeup();
    vTaskDelay(pdMS_TO_TICKS(50UL));
    hih8120_measure();
    vTaskDelay(pdMS_TO_TICKS(1UL));
    int16_t temperature = (int16_t)(hih8120_getTemperature()*10)+200;
    if (temperature < 0) temperature = 0;
    if (temperature > 1023) temperature = 1023;
    sensorData->totalTemperature += temperature;
    sensorData->counter++;
    printf("totalTemperature: %d\ncounter: %d\n", temperature, sensorData->counter);
    
}

void sensorData_reset(){    
    sensorData->totalTemperature = 0;
    sensorData->totalHumidity = 0;
    sensorData->totalCarbondioxide = 0;
    sensorData->counter = 0;
}

uint16_t sensorData_getTemperatureAverage() {
    return sensorData->totalTemperature/sensorData->counter;
}

uint16_t sensorData_getHumidityAverage() {
    return sensorData->totalHumidity/sensorData->counter;
}

uint16_t sensorData_getCarbondioxideAverage() {
    return sensorData->totalCarbondioxide/sensorData->counter;
}