#include "gtest/gtest.h"
#include "fff.h"
#include "FreeRTOS_FFF_MocksDeclaration.h"

// Include interfaces and define global variables
// defined by the production code
extern "C" {
    #include "LoRaWANUtil.h"
    #include "SensorData.h"
    #include "TemperatureTransmit.h"
}


FAKE_VOID_FUNC(LoRaWANUtil_setup);
FAKE_VOID_FUNC(LoRaWANUtil_sendPayload, lora_driver_payload_t*);
// FAKE_VOID_FUNC(lora_driver_resetRn2483,uint8_t);
// FAKE_VOID_FUNC(lora_driver_flushBuffers);
// FAKE_VOID_FUNC(status_leds_slowBlink,status_leds_t);
// FAKE_VALUE_FUNC(char*, lora_driver_mapReturnCodeToText, lora_driver_returnCode_t);
// FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_rn2483FactoryReset);
// FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_configureToEu868);
// FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_getRn2483Hweui,char*);
// FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_setDeviceIdentifier,const char*);
// FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_setOtaaIdentity, char*, char*, char*);
// FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_saveMac);
// FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_setAdaptiveDataRate, lora_driver_adaptiveDataRate_t);
// FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_setReceiveDelay, uint16_t);
// FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_join, lora_driver_joinMode_t);
// FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_sendUploadMessage, bool, lora_driver_payload_t*);
// FAKE_VOID_FUNC(status_leds_longPuls, status_leds_t);
// FAKE_VOID_FUNC(status_leds_ledOn, status_leds_t);
// FAKE_VOID_FUNC(status_leds_ledOff, status_leds_t);
// FAKE_VOID_FUNC(status_leds_fastBlink, status_leds_t);
//FAKE_VALUE_FUNC(uint16_t,sensorData_getTemperatureAverage,sensorData_t);
//FAKE_VOID_FUNC(loRaWANSetup);
//FAKE_VOID_FUNC(taskYIELD);
class TempTestFixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        RESET_FAKE(LoRaWANUtil_sendPayload);
        RESET_FAKE(xTaskCreate);
        RESET_FAKE(xSemaphoreTake);
        RESET_FAKE(xSemaphoreGive);
        RESET_FAKE(xTaskGetTickCount);
        RESET_FAKE(xTaskDelayUntil);
        FFF_RESET_HISTORY();
    }
    void TearDown() override
    {}
};

TEST_F(TempTestFixture, testInit){
    sensorData_t data = sensorData_init();
    temperatureTransmit_taskInit(data);
    sensorData_destroy(data);
    ASSERT_EQ(1,1);
}

TEST_F(TempTestFixture, testrun){
    sensorData_t data = sensorData_init();
    data->totalTemperature=450;
    data->counter=1;
    temperatureTransmit_taskInit(data);
    temperatureTransmit_taskRun();
    sensorData_destroy(data);
}

TEST_F(TempTestFixture,testValueOfSensorDataAfterRun){
    sensorData_t data = sensorData_init();
    data->totalTemperature=450;
    data->counter=1;
    temperatureTransmit_taskInit(data);
    temperatureTransmit_taskRun();

    ASSERT_EQ(data->totalTemperature,0);
    ASSERT_EQ(data->counter,0);        
    sensorData_destroy(data);
}

TEST_F(TempTestFixture,testValueOfTTempInPayload){
    sensorData_t data = sensorData_init();
    data->totalTemperature=450;
    data->counter=1;
    temperatureTransmit_taskInit(data);
    temperatureTransmit_taskRun();

    ASSERT_EQ(LoRaWANUtil_sendPayload_fake.arg0_val->bytes[0],(uint8_t) 154);
    ASSERT_EQ(LoRaWANUtil_sendPayload_fake.arg0_val->bytes[1] & 0b11000000,(uint8_t) 64);
    sensorData_destroy(data);
}