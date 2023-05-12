#include "gtest/gtest.h"
#include "fff.h"
#include "FreeRTOS_FFF_MocksDeclaration.h"

// Include interfaces and define global variables
// defined by the production code
extern "C"
{
	#include "SensorData.h"
	#include "hih8120.h"
}

FAKE_VALUE_FUNC(hih8120_driverReturnCode_t, hih8120_measure);
FAKE_VALUE_FUNC(hih8120_driverReturnCode_t, hih8120_wakeup);
FAKE_VALUE_FUNC(int16_t, hih8120_getTemperature_x10);

// Create Test fixture and Reset all Mocks before each test
class Test_fixture : public ::testing::Test
{
protected:
	void SetUp() override
	{
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

// Test that the task is created correct
TEST_F(Test_fixture, Test_createTask1){
    ASSERT_EQ(1,1);
}