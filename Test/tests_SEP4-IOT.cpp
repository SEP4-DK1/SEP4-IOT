#include "gtest/gtest.h"
#include "fff.h"
#include "FreeRTOS_FFF_MocksDeclaration.h"

// Include interfaces and define global variables
// defined by the production code
extern "C"
{
    #include "SensorData.h"
    #include "hih8120.h"
    #include "mh_z19.h"
}

// Declare FFF mocks for the production code functions
FAKE_VALUE_FUNC(hih8120_driverReturnCode_t, hih8120_measure);
FAKE_VALUE_FUNC(hih8120_driverReturnCode_t, hih8120_wakeup);
FAKE_VALUE_FUNC(uint16_t, hih8120_getTemperature_x10);
FAKE_VALUE_FUNC(mh_z19_returnCode_t, mh_z19_takeMeassuring);
FAKE_VALUE_FUNC(mh_z19_returnCode_t, mh_z19_getCo2Ppm, uint16_t*);

// Create Test fixture and Reset all Mocks before each test
class Test_fixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        RESET_FAKE(hih8120_measure);
        RESET_FAKE(hih8120_wakeup);
        RESET_FAKE(hih8120_getTemperature_x10);
        RESET_FAKE(mh_z19_takeMeassuring);
        RESET_FAKE(mh_z19_getCo2Ppm);
        FFF_RESET_HISTORY();
    }
    void TearDown() override
    {}
};
