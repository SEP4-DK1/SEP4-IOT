#pragma once
#include <stdint.h>

typedef struct sensorData
{
    uint16_t totalTemperature;
    uint16_t totalHumidity;
    uint16_t totalCarbondioxide;
    uint16_t counter;
} * sensorData_t;

sensorData_t sensorData_init();
void sensorData_destroy(sensorData_t data);

void sensorData_measure(sensorData_t data);
void sensorData_reset(sensorData_t data);

uint16_t sensorData_getTemperatureAverage(sensorData_t data);
uint16_t sensorData_getHumidityAverage(sensorData_t data);
uint16_t sensorData_getCarbondioxideAverage(sensorData_t data);
