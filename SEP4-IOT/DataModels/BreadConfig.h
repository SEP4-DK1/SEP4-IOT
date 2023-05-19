#pragma once
#include <stdint.h>
#include "SensorDefinitions.h"

typedef struct breadConfig {
	int16_t temperature;
	int16_t humidity;
} * breadConfig_t;

breadConfig_t breadConfig_init();
void breadConfig_destroy(breadConfig_t breadConfig);