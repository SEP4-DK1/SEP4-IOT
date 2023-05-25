#pragma once
#include <stdint.h>
#include "SensorDefinitions.h"
#define TEMPERATURE_CALIBRATION -40

typedef struct sensorData {
  uint16_t latestTemperature;
  uint16_t latestHumidity;
  uint16_t latestCarbondioxide;
  uint32_t totalTemperature;
  uint32_t totalHumidity;
  uint32_t totalCarbondioxide;
  uint16_t co2Counter;
  uint16_t tempHumCounter;
} * sensorData_t;

sensorData_t sensorData_init();
void sensorData_destroy(sensorData_t data);

void sensorData_hih8120Measure(sensorData_t data);
void sensorData_mhz19Measure(sensorData_t data);
void sensorData_reset(sensorData_t data);

uint16_t sensorData_getTemperatureAverage(sensorData_t data);
uint16_t sensorData_getHumidityAverage(sensorData_t data);
uint16_t sensorData_getCarbondioxideAverage(sensorData_t data);
