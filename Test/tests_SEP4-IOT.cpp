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

FAKE_VALUE_FUNC(hih8120_driverReturnCode_t, hih8120_measure);
FAKE_VALUE_FUNC(hih8120_driverReturnCode_t, hih8120_wakeup);
FAKE_VALUE_FUNC(int16_t, hih8120_getTemperature_x10);
FAKE_VALUE_FUNC(uint16_t, hih8120_getHumidityPercent_x10)
FAKE_VALUE_FUNC(mh_z19_returnCode_t, mh_z19_takeMeassuring);
FAKE_VALUE_FUNC(mh_z19_returnCode_t, mh_z19_getCo2Ppm, uint16_t*);

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

TEST_F(Test_fixture, Test_sensorDataMeasureTemperature)
{
	// Create sensorData struct
	sensorData_t data = sensorData_init();

	// Set up fake values for hih8120_measure and hih8120_getTemperature_x10
	hih8120_measure_fake.return_val = HIH8120_OK;
	hih8120_getTemperature_x10_fake.return_val = 250;


	// Call sensorData_measure
	sensorData_measure(data);

	// Check that the values have been updated correctly
	ASSERT_EQ(data->totalTemperature, 250);
	ASSERT_EQ(data->counter, 1);

	// Clean up
	sensorData_destroy(data);
}

TEST_F(Test_fixture, Test_sensorDataMeasureHumidity)
{
    // Create sensorData struct
    sensorData_t data = sensorData_init();

    // Set up fake values for hih8120_measure and hih8120_getHumidityPercent_x10
    hih8120_measure_fake.return_val = HIH8120_OK;
    hih8120_getHumidityPercent_x10_fake.return_val = 500; 

    // Call sensorData_measure
    sensorData_measure(data);
    // Check that the values have been updated correctly
    ASSERT_EQ(data->totalHumidity, 50);
    ASSERT_EQ(data->counter, 1);

    // Clean up
    sensorData_destroy(data);
}

TEST_F(Test_fixture, Test_sensorDataMeasureCo2)
{
    // Create sensorData struct
    sensorData_t data = sensorData_init();

    // Set up fake values
    mh_z19_takeMeassuring_fake.return_val = MHZ19_OK;
    
	mh_z19_getCo2Ppm_fake.custom_fake = [](uint16_t *ppm) {
        *ppm = 100;
        return MHZ19_OK;
    };

    // Call sensorData_measure
    sensorData_measure(data);

    // Check that the values have been updated correctly
    ASSERT_EQ(data->totalCarbondioxide, 100); 
    ASSERT_EQ(data->counter, 1);

    // Clean up
    sensorData_destroy(data);
}

TEST_F(Test_fixture, Test_sensorDataTemperatureAverage)
{
	// Create sensorData struct and set initial values
	sensorData_t data = sensorData_init();
	data->totalTemperature = 5000;
	data->counter = 5;

	// Call sensorData_getTemperatureAverage
	uint16_t average = sensorData_getTemperatureAverage(data);

	// Check that the average is calculated correctly
	ASSERT_EQ(average, 1000-40);

	// Clean up
	sensorData_destroy(data);
}

TEST_F(Test_fixture, Test_sensorDataReset)
{
	// Create sensorData struct and set initial values
	sensorData_t data = sensorData_init();
	data->totalTemperature = 100;
	data->totalHumidity = 200;
	data->totalCarbondioxide = 300;
	data->counter = 4;

	// Call sensorData_reset
	sensorData_reset(data);

	// Check that the values have been reset to zero
	ASSERT_EQ(data->totalTemperature, 0);
	ASSERT_EQ(data->totalHumidity, 0);
	ASSERT_EQ(data->totalCarbondioxide, 0);
	ASSERT_EQ(data->counter, 0);

	// Clean up
	sensorData_destroy(data);
}

TEST_F(Test_fixture, Test_sensorDataHumidityAverage)
{
	// Create sensorData struct and set initial values
	sensorData_t data = sensorData_init();
	data->totalHumidity = 300;
	data->counter = 3;

	// Call sensorData_getHumidityAverage
	uint16_t average = sensorData_getHumidityAverage(data);

	// Check that the average is calculated correctly
	ASSERT_EQ(average, 100);

	// Clean up
	sensorData_destroy(data);
}

TEST_F(Test_fixture, Test_sensorDataCarbondioxideAverage)
{
	// Create sensorData struct and set initial values
	sensorData_t data = sensorData_init();
	data->totalCarbondioxide = 150;
	data->counter = 2;

	// Call sensorData_getCarbondioxideAverage
	uint16_t average = sensorData_getCarbondioxideAverage(data);

	// Check that the average is calculated correctly
	ASSERT_EQ(average, 75);

	// Clean up
	sensorData_destroy(data);
}