#include "SensorData.h"
#include "SensorDefinitions.h"
#include <ATMEGA_FreeRTOS.h>
#include <hih8120.h>
#include <mh_z19.h>
#include <stdio.h>
#include <stdlib.h>

sensorData_t sensorData_init() {
  sensorData_t data; 
  data = malloc(sizeof(*data));
  data->totalTemperature = 0;
  data->totalHumidity = 0;
  data->totalCarbondioxide = 0;
  data->co2Counter = 0;
  data->tempHumCounter = 0;
  return data;
}

void sensorData_destroy(sensorData_t data){
  if (data != NULL) {
    free(data);
  }
}

void sensorData_hih8120Measure(sensorData_t data){
  if (hih8120_wakeup() != HIH8120_OK) {
    printf("ERROR: Wakeup HIH8120 failed\n");
    return;
  }
  vTaskDelay(pdMS_TO_TICKS(50UL));
  if (hih8120_measure() != HIH8120_OK) {
    printf("ERROR: Measure HIH8120 failed\n");
    return;
  }
  vTaskDelay(pdMS_TO_TICKS(1UL));
  int16_t temperature = hih8120_getTemperature_x10() + TEMPERATURE_OFFSET + TEMPERATURE_CALIBRATION;
  uint16_t humidity = hih8120_getHumidityPercent_x10()/10;

  if (temperature < 0) temperature = 0;
  if (temperature > 1023) temperature = 1023;
  data->latestTemperature = temperature;
  data->totalTemperature += temperature;

  if (humidity < 0) humidity = 0;
  if (humidity > 100) humidity = 100;
  data->totalHumidity += humidity;
    
  data->tempHumCounter++;
}

void sensorData_mhz19Measure(sensorData_t data){
  if (mh_z19_takeMeassuring() != MHZ19_OK){
    printf("ERROR: Measure mh_z19 failed\n");
    return;
  }
  vTaskDelay(pdMS_TO_TICKS(50UL));
  uint16_t carbondioxcide = 0;
  
  if (mh_z19_getCo2Ppm(&carbondioxcide) != MHZ19_OK){
    printf("ERROR: GetCo2Ppm failed\n");
    return;
  }

  data->totalCarbondioxide += carbondioxcide;
  
  data->co2Counter++;
}

void sensorData_reset(sensorData_t data){    
  data->totalTemperature = 0;
  data->totalHumidity = 0;
  data->totalCarbondioxide = 0;
  data->co2Counter = 0;
  data->tempHumCounter = 0;
}

uint16_t sensorData_getTemperatureAverage(sensorData_t data) {
  if (data->tempHumCounter != 0) return (data->totalTemperature / data->tempHumCounter);
  return 0;
}

uint16_t sensorData_getHumidityAverage(sensorData_t data) {
  if (data->tempHumCounter != 0) return data->totalHumidity / data->tempHumCounter;
  return 0;
}

uint16_t sensorData_getCarbondioxideAverage(sensorData_t data) {
  if (data->co2Counter != 0) return data->totalCarbondioxide / data->co2Counter;
  return 0;
}
