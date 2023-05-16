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

TEST_F(TempTestFixture,testValueOfTemperatureInPayload){
    sensorData_t data = sensorData_init();
    data->totalTemperature=450;
    data->counter=1;
    temperatureTransmit_taskInit(data);
    temperatureTransmit_taskRun();

    ASSERT_EQ(LoRaWANUtil_sendPayload_fake.arg0_val->bytes[0],(uint8_t) 154);
    ASSERT_EQ(LoRaWANUtil_sendPayload_fake.arg0_val->bytes[1] & 0b11000000,(uint8_t) 64);
    sensorData_destroy(data);
}

TEST_F(TempTestFixture,testValueOfHumidityInPayload){
    sensorData_t data = sensorData_init();
    data->totalHumidity=100;
    data->counter=1;
    temperatureTransmit_taskInit(data);
    temperatureTransmit_taskRun();

    ASSERT_EQ(LoRaWANUtil_sendPayload_fake.arg0_val->bytes[1] & 0b00111111,(uint8_t) 36);
    ASSERT_EQ(LoRaWANUtil_sendPayload_fake.arg0_val->bytes[2] & 0b10000000,(uint8_t) 128);
    sensorData_destroy(data);
}

TEST_F(TempTestFixture,testValueOfCO2InPayload){
    sensorData_t data = sensorData_init();
    data->totalHumidity=4965;
    data->counter=1;
    temperatureTransmit_taskInit(data);
    temperatureTransmit_taskRun();

    ASSERT_EQ(LoRaWANUtil_sendPayload_fake.arg0_val->bytes[2] & 0b01111111,(uint8_t) 101);
    ASSERT_EQ(LoRaWANUtil_sendPayload_fake.arg0_val->bytes[3] & 0b11111100,(uint8_t) 152);
    sensorData_destroy(data);
}