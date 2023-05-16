#define TEMPERATURE_OFFSET 200
// TEMPERATURE_OFFSET is used to offset the temperature
// A temperature of -20°C is equivalent to 0 in our code.
// This is done to avoid sending negative numbers
// It also allows the temperature to only use 10 bits, which is practical for LoRaWAN
// Temperature range 0-1023 (-20° to 82.3°)