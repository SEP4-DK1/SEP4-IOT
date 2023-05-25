#include "gtest/gtest.h"
#include "fff.h"
#include "FreeRTOS_FFF_MocksDeclaration.h"

// Include interfaces and define global variables
// defined by the production code
extern "C"
{
	#include "SensorData.h"
  #include "../SEP4-IOT/Tasks/ClimateControl.h"
  #include "BreadConfig.h"
  #include "rc_servo.h"
}

FAKE_VOID_FUNC(rc_servo_setPosition, uint8_t, int8_t);

class TestClimateControl : public ::testing::Test
{
protected:
	void SetUp() override
	{
    RESET_FAKE(rc_servo_setPosition);
		RESET_FAKE(xTaskCreate);
		RESET_FAKE(xSemaphoreTake);
		RESET_FAKE(xSemaphoreGive);
		RESET_FAKE(xTaskGetTickCount);
		RESET_FAKE(xTaskDelayUntil);
		FFF_RESET_HISTORY();

    xSemaphoreTake_fake.return_val = pdTRUE;
	}
	void TearDown() override
	{}
};

TEST_F(TestClimateControl,taskinit) {
  // Set up the test data
  SemaphoreHandle_t sensorDataMutex = xSemaphoreCreateMutex();
  SemaphoreHandle_t breadConfigMutex = xSemaphoreCreateMutex();
  sensorData_t sensorData = sensorData_init();
  breadConfig_t breadConfig = breadConfig_init();

  climateControlParams_t params = climateControl_createParams(sensorDataMutex, breadConfigMutex, sensorData, breadConfig);

  // Call the function under test
  climateControl_taskInit(params);

  // Clean up
  sensorData_destroy(sensorData);
  breadConfig_destroy(breadConfig);
}

TEST_F(TestClimateControl,taskrun) {
  // Set up the test data
  SemaphoreHandle_t sensorDataMutex = xSemaphoreCreateMutex();
  SemaphoreHandle_t breadConfigMutex = xSemaphoreCreateMutex();
  sensorData_t sensorData = sensorData_init();
  breadConfig_t breadConfig = breadConfig_init();

  climateControlParams_t params = climateControl_createParams(sensorDataMutex, breadConfigMutex, sensorData, breadConfig);

  // Call the function under test
  climateControl_taskInit(params);
  climateControl_taskRun();

  // Clean up
  sensorData_destroy(sensorData);
  breadConfig_destroy(breadConfig);
}

TEST_F(TestClimateControl, testTemperature210HeatoffWithConfig200Temperature) {
  // Set up the test data
  SemaphoreHandle_t sensorDataMutex = xSemaphoreCreateMutex();
  SemaphoreHandle_t breadConfigMutex = xSemaphoreCreateMutex();
  sensorData_t sensorData = sensorData_init();
  breadConfig_t breadConfig = breadConfig_init();
  sensorData->latestTemperature = 315; 
  breadConfig->temperature = 200;

  climateControlParams_t params = climateControl_createParams(sensorDataMutex, breadConfigMutex, sensorData, breadConfig);

  // Call the function under test
  climateControl_taskInit(params);
  climateControl_taskRun();

  // Verify the expected behavior
  EXPECT_EQ(rc_servo_setPosition_fake.arg0_history[0], SERVO0); 
  EXPECT_EQ(rc_servo_setPosition_fake.arg1_history[0], HEATEROFF);

  // Clean up
  sensorData_destroy(sensorData);
  breadConfig_destroy(breadConfig);
}

TEST_F(TestClimateControl, testTemperature605Heats12percentWithConfig600Temperature) {
  // Set up the test data
  SemaphoreHandle_t sensorDataMutex = xSemaphoreCreateMutex();
  SemaphoreHandle_t breadConfigMutex = xSemaphoreCreateMutex();
  sensorData_t sensorData = sensorData_init();
  breadConfig_t breadConfig = breadConfig_init();
  sensorData->latestTemperature = 605; 
  breadConfig->temperature = 600;

  climateControlParams_t params = climateControl_createParams(sensorDataMutex, breadConfigMutex, sensorData, breadConfig);

  // Call the function under test
  climateControl_taskInit(params);
  climateControl_taskRun();

  // Verify the expected behavior
  EXPECT_EQ(rc_servo_setPosition_fake.arg0_history[0], SERVO0); 
  EXPECT_EQ(rc_servo_setPosition_fake.arg1_history[0], HEATER12PERCENT);

  // Clean up
  sensorData_destroy(sensorData);
  breadConfig_destroy(breadConfig);
}


TEST_F(TestClimateControl, testTemperature200Heats25percentWithConfig400Temperature) {
  // Set up the test data
  SemaphoreHandle_t sensorDataMutex = xSemaphoreCreateMutex();
  SemaphoreHandle_t breadConfigMutex = xSemaphoreCreateMutex();
  sensorData_t sensorData = sensorData_init();
  breadConfig_t breadConfig = breadConfig_init();
  sensorData->latestTemperature = 200; 
  breadConfig->temperature = 400;

  climateControlParams_t params = climateControl_createParams(sensorDataMutex, breadConfigMutex, sensorData, breadConfig);

  // Call the function under test
  climateControl_taskInit(params);
  climateControl_taskRun();

  // Verify the expected behavior
  EXPECT_EQ(rc_servo_setPosition_fake.arg0_history[0], SERVO0); 
  EXPECT_EQ(rc_servo_setPosition_fake.arg1_history[0], HEATER75PERCENT);

  // Clean up
  sensorData_destroy(sensorData);
  breadConfig_destroy(breadConfig);
}

TEST_F(TestClimateControl, testTemperature500Heats50percentWithConfig600Temperature) {
  // Set up the test data
  SemaphoreHandle_t sensorDataMutex = xSemaphoreCreateMutex();
  SemaphoreHandle_t breadConfigMutex = xSemaphoreCreateMutex();
  sensorData_t sensorData = sensorData_init();
  breadConfig_t breadConfig = breadConfig_init();
  sensorData->latestTemperature = 500; 
  breadConfig->temperature = 600;

  climateControlParams_t params = climateControl_createParams(sensorDataMutex, breadConfigMutex, sensorData, breadConfig);

  // Call the function under test
  climateControl_taskInit(params);
  climateControl_taskRun();

  // Verify the expected behavior
  EXPECT_EQ(rc_servo_setPosition_fake.arg0_history[0], SERVO0); 
  EXPECT_EQ(rc_servo_setPosition_fake.arg1_history[0], HEATER50PERCENT);

  // Clean up
  sensorData_destroy(sensorData);
  breadConfig_destroy(breadConfig);
}

TEST_F(TestClimateControl, testTemperature400Heats75percentWithConfig600Temperature) {
  // Set up the test data
  SemaphoreHandle_t sensorDataMutex = xSemaphoreCreateMutex();
  SemaphoreHandle_t breadConfigMutex = xSemaphoreCreateMutex();
  sensorData_t sensorData = sensorData_init();
  breadConfig_t breadConfig = breadConfig_init();
  sensorData->latestTemperature = 400; 
  breadConfig->temperature = 600;

  climateControlParams_t params = climateControl_createParams(sensorDataMutex, breadConfigMutex, sensorData, breadConfig);

  // Call the function under test
  climateControl_taskInit(params);
  climateControl_taskRun();

  // Verify the expected behavior
  EXPECT_EQ(rc_servo_setPosition_fake.arg0_history[0], SERVO0); 
  EXPECT_EQ(rc_servo_setPosition_fake.arg1_history[0], HEATER75PERCENT); 

  // Clean up
  sensorData_destroy(sensorData);
  breadConfig_destroy(breadConfig);
}

TEST_F(TestClimateControl, testTemperature300Heats100percentWithConfig600Temperature) {
  // Set up the test data
  SemaphoreHandle_t sensorDataMutex = xSemaphoreCreateMutex();
  SemaphoreHandle_t breadConfigMutex = xSemaphoreCreateMutex();
  sensorData_t sensorData = sensorData_init();
  breadConfig_t breadConfig = breadConfig_init();
  sensorData->latestTemperature = 300; 
  breadConfig->temperature = 600;

  climateControlParams_t params = climateControl_createParams(sensorDataMutex, breadConfigMutex, sensorData, breadConfig);

  // Call the function under test
  climateControl_taskInit(params);
  climateControl_taskRun();

  // Verify the expected behavior
  EXPECT_EQ(rc_servo_setPosition_fake.arg0_history[0], SERVO0); 
  EXPECT_EQ(rc_servo_setPosition_fake.arg1_history[0], HEATER100PERCENT); 

  // Clean up
  sensorData_destroy(sensorData);
  breadConfig_destroy(breadConfig);
}

TEST_F(TestClimateControl, testTemperature400cOpenVentilationWithConfig370Temperature) {
  // Set up the test data
  SemaphoreHandle_t sensorDataMutex = xSemaphoreCreateMutex();
  SemaphoreHandle_t breadConfigMutex = xSemaphoreCreateMutex();
  sensorData_t sensorData = sensorData_init();
  breadConfig_t breadConfig = breadConfig_init();
  sensorData->latestTemperature = 400; 
  breadConfig->temperature = 370;

  climateControlParams_t params = climateControl_createParams(sensorDataMutex, breadConfigMutex, sensorData, breadConfig);

  // Call the function under test
  climateControl_taskInit(params);
  climateControl_taskRun();

  // Verify the expected behavior
  EXPECT_EQ(rc_servo_setPosition_fake.arg0_history[1], SERVO1); 
  EXPECT_EQ(rc_servo_setPosition_fake.arg1_history[1], VENTILATIONOPEN); 

  // Clean up
  sensorData_destroy(sensorData);
  breadConfig_destroy(breadConfig);
}


TEST_F(TestClimateControl, testHumidity90OpenVentilation80WithConfig80PercentHumidity) {
  // Set up the test data
  SemaphoreHandle_t sensorDataMutex = xSemaphoreCreateMutex();
  SemaphoreHandle_t breadConfigMutex = xSemaphoreCreateMutex();
  sensorData_t sensorData = sensorData_init();
  breadConfig_t breadConfig = breadConfig_init();
  sensorData->latestHumidity = 90; 
  breadConfig->humidity = 75;

  climateControlParams_t params = climateControl_createParams(sensorDataMutex, breadConfigMutex, sensorData, breadConfig);

  // Call the function under test
  climateControl_taskInit(params);
  climateControl_taskRun();

  // Verify the expected behavior
  EXPECT_EQ(rc_servo_setPosition_fake.arg0_history[1], SERVO1); 
  EXPECT_EQ(rc_servo_setPosition_fake.arg1_history[1], VENTILATIONOPEN); 

  // Clean up
  sensorData_destroy(sensorData);
  breadConfig_destroy(breadConfig);
}

TEST_F(TestClimateControl, testOpenVentilationWithC02AboveLimit) {
  // Set up the test data
  SemaphoreHandle_t sensorDataMutex = xSemaphoreCreateMutex();
  SemaphoreHandle_t breadConfigMutex = xSemaphoreCreateMutex();
  sensorData_t sensorData = sensorData_init();
  breadConfig_t breadConfig = breadConfig_init();
  breadConfig->temperature = 400;
  breadConfig->humidity = 60;
  sensorData->latestCarbondioxide = CO2LIMIT + 100;


  climateControlParams_t params = climateControl_createParams(sensorDataMutex, breadConfigMutex, sensorData, breadConfig);

  // Call the function under test
  climateControl_taskInit(params);
  climateControl_taskRun();

  // Verify the expected behavior
  EXPECT_EQ(rc_servo_setPosition_fake.arg0_history[1], SERVO1); 
  EXPECT_EQ(rc_servo_setPosition_fake.arg1_history[1], VENTILATIONOPEN); 

  // Clean up
  sensorData_destroy(sensorData);
  breadConfig_destroy(breadConfig);
}

TEST_F(TestClimateControl, testflashBoilHumidity100AndConfig120) {
  // Set up the test data
  SemaphoreHandle_t sensorDataMutex = xSemaphoreCreateMutex();
  SemaphoreHandle_t breadConfigMutex = xSemaphoreCreateMutex();
  sensorData_t sensorData = sensorData_init();
  breadConfig_t breadConfig = breadConfig_init();
  breadConfig->humidity = 120;
  sensorData->latestHumidity = 100;

  climateControlParams_t params = climateControl_createParams(sensorDataMutex, breadConfigMutex, sensorData, breadConfig);

  // Call the function under test
  climateControl_taskInit(params);
  climateControl_taskRun();

  // Verify the expected behavior
  EXPECT_EQ(rc_servo_setPosition_fake.arg0_history[2], SERVO0); 
  EXPECT_EQ(rc_servo_setPosition_fake.arg1_history[2], HEATER100PERCENT); 
  EXPECT_EQ(rc_servo_setPosition_fake.arg0_history[3], SERVO0); 
  EXPECT_EQ(rc_servo_setPosition_fake.arg1_history[3], HEATER12PERCENT); 

  // Clean up
  sensorData_destroy(sensorData);
  breadConfig_destroy(breadConfig);
}