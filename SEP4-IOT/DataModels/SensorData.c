#include "SensorData.h"
#include <ATMEGA_FreeRTOS.h>
#include <hih8120.h>
#include <mh_z19.h>
#include <stdio.h>
#include <stdlib.h>

#define TEMPERATURE_OFFSET 40

sensorData_t sensorData_init() {
    
    sensorData_t data; 
    data->totalTemperature = 0;
    data->totalHumidity = 0;
    data->totalCarbondioxide = 0;
    data->counter = 0;
    return data;
}

void sensorData_destroy(sensorData_t data){
    if (data != NULL)
    {
        free(data);
    }
}

void sensorData_measure(sensorData_t data){
    if (hih8120_wakeup() != HIH8120_OK) {
        printf("ERROR: Wakeup HIH8120 failed\n");
    }
    vTaskDelay(pdMS_TO_TICKS(50UL));
    if (hih8120_measure() != HIH8120_OK) {
        printf("ERROR: Measure HIH8120 failed\n");
    }
    if (mh_z19_takeMeassuring() != MHZ19_OK){
        printf("ERROR: Measure mh_z19 failed\n");
    }
    
    vTaskDelay(pdMS_TO_TICKS(1UL));
    int16_t temperature = hih8120_getTemperature_x10();
    if (temperature < 0) temperature = 0;
    if (temperature > 1023) temperature = 1023;
    data->totalTemperature += temperature;
    uint16_t carbondioxcide = mh_z19_getCo2Ppm(&carbondioxcide);
    data->totalCarbondioxide += carbondioxcide;

    data->counter++;
}

void sensorData_reset(sensorData_t data){    
    data->totalTemperature = 0;
    data->totalHumidity = 0;
    data->totalCarbondioxide = 0;
    data->counter = 0;
}

uint16_t sensorData_getTemperatureAverage(sensorData_t data) {
    return (data->totalTemperature / data->counter) - TEMPERATURE_OFFSET; // Maybe subtract the temperature offset somewhere else?
}

uint16_t sensorData_getHumidityAverage(sensorData_t data) {
    return data->totalHumidity / data->counter;
}

uint16_t sensorData_getCarbondioxideAverage(sensorData_t data) {
    return data->totalCarbondioxide / data->counter;
}