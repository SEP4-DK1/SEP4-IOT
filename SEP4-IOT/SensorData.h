#pragma once
#include <stdint.h>

typedef struct sensorData
{
    uint16_t totalTemperature;
    uint16_t totalHumidity;
    uint16_t totalCarbondioxide;
    uint16_t counter;
} * sensorData_t;

void sensorData_measure();
void sensorData_reset();

uint16_t sensorData_getTemperatureAverage();
uint16_t sensorData_getHumidityAverage();
uint16_t sensorData_getCarbondioxideAverage();
