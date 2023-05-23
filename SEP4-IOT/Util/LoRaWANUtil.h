#pragma once
#include <lora_driver.h>

void LoRaWANUtil_setup(void);
void LoRaWANUtil_sendPayload(lora_driver_payload_t *_uplink_payload);