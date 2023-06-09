#include "LoRaWANUtil.h"

#include <stdio.h>
#include <stdint.h>
#include <ATMEGA_FreeRTOS.h>

#include "../Config/LoraWAN_Config.h"
#include <status_leds.h>

void LoRaWANUtil_setup(void) {
  // Hardware reset of LoRaWAN transceiver
  lora_driver_resetRn2483(1);
  vTaskDelay(2);
  lora_driver_resetRn2483(0);
  // Give it a chance to wakeup
  vTaskDelay(150);

  lora_driver_flushBuffers(); // get rid of first version string from module after reset!

  char _out_buf[20];
  lora_driver_returnCode_t rc;
  status_leds_slowBlink(led_ST2); // OPTIONAL: Led the green led blink slowly while we are setting up LoRa

  // Factory reset the transceiver
  printf("FactoryReset >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_rn2483FactoryReset()));
  
  // Configure to EU868 LoRaWAN standards
  printf("Configure to EU868 >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_configureToEu868()));

  // Get the transceivers HW EUI
  rc = lora_driver_getRn2483Hweui(_out_buf);
  printf("Get HWEUI >%s<: %s\n",lora_driver_mapReturnCodeToText(rc), _out_buf);

  // Set the HWEUI as DevEUI in the LoRaWAN software stack in the transceiver
  printf("Set DevEUI: %s >%s<\n", _out_buf, lora_driver_mapReturnCodeToText(lora_driver_setDeviceIdentifier(_out_buf)));

  // Set Over The Air Activation parameters to be ready to join the LoRaWAN
  printf("Set OTAA Identity appEUI:%s appKEY:%s devEUI:%s >%s<\n", LORA_appEUI, LORA_appKEY, _out_buf, lora_driver_mapReturnCodeToText(lora_driver_setOtaaIdentity(LORA_appEUI,LORA_appKEY,_out_buf)));

  // Save all the MAC settings in the transceiver
  printf("Save mac >%s<\n",lora_driver_mapReturnCodeToText(lora_driver_saveMac()));

  // Enable Adaptive Data Rate
  printf("Set Adaptive Data Rate: ON >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_setAdaptiveDataRate(LORA_ON)));

  // Set receiver window1 delay to 500 ms - this is needed if down-link messages will be used
  printf("Set Receiver Delay: %d ms >%s<\n", 500, lora_driver_mapReturnCodeToText(lora_driver_setReceiveDelay(500)));

  // Join the LoRaWAN
  uint8_t maxJoinTriesLeft = 10;
  
  do {
    rc = lora_driver_join(LORA_OTAA);
    printf("Join Network TriesLeft:%d >%s<\n", maxJoinTriesLeft, lora_driver_mapReturnCodeToText(rc));

    if ( rc != LORA_ACCEPTED) {
      // Make the red led pulse to tell something went wrong
      status_leds_longPuls(led_ST1); // OPTIONAL
      // Wait 5 sec and lets try again
      vTaskDelay(pdMS_TO_TICKS(5000UL));
    }
    else {
      break;
    }
  } while (--maxJoinTriesLeft);

  if (rc == LORA_ACCEPTED) {
    // Connected to LoRaWAN :-)
    // Make the green led steady
    status_leds_ledOn(led_ST2); // OPTIONAL
    printf("Successfully connected to LoRaWAN\n");
  }
  else {
    // Something went wrong
    // Turn off the green led
    status_leds_ledOff(led_ST2); // OPTIONAL
    // Make the red led blink fast to tell something went wrong
    status_leds_fastBlink(led_ST1); // OPTIONAL
    printf("Could not connect to LoRaWAN");

    // Lets stay here
    while (1) {
      //taskYIELD();
    }
  }
}

void LoRaWANUtil_sendPayload(lora_driver_payload_t *_uplink_payload) {
  printf("Upload Message >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_sendUploadMessage(false, _uplink_payload)));
}