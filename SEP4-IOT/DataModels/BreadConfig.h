#pragma once
#include <stdint.h>
#include "SensorDefinitions.h"

typedef struct breadConfig {
  uint16_t temperature;
  uint8_t humidity;
} * breadConfig_t;

breadConfig_t breadConfig_init(void);
void breadConfig_destroy(breadConfig_t breadConfig);