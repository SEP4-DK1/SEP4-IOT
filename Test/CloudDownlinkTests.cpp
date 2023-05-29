#include "gtest/gtest.h"
#include "fff.h"
#include "FreeRTOS_FFF_MocksDeclaration.h"

// Include interfaces and define global variables
// defined by the production code
extern "C" {
  #include "BreadConfig.h"
  #include "CloudDownlink.h"
}

class DownlinkTestFixture : public ::testing::Test {
protected:
  void SetUp() override {
    RESET_FAKE(xMessageBufferReceive);
    RESET_FAKE(xTaskCreate);
    RESET_FAKE(xSemaphoreTake);
    RESET_FAKE(xSemaphoreGive);
    RESET_FAKE(xTaskGetTickCount);
    RESET_FAKE(xTaskDelayUntil);
    FFF_RESET_HISTORY();

    xSemaphoreTake_fake.return_val = pdTRUE;
  }
  void TearDown() override {}
};

TEST_F(DownlinkTestFixture, testInit) {
  breadConfig_t breadConfig = breadConfig_init();
  MessageBufferHandle_t messageBufferHandle = xMessageBufferCreate(sizeof(lora_driver_payload_t));
  SemaphoreHandle_t breadConfigMutex = xSemaphoreCreateMutex();
  cloudDownlinkParams_t params = cloudDownlink_createParams(breadConfigMutex, messageBufferHandle, breadConfig);
  
  cloudDownlink_taskInit(params);
  breadConfig_destroy(breadConfig);
}

TEST_F(DownlinkTestFixture, testrun) {
  breadConfig_t breadConfig = breadConfig_init();
  MessageBufferHandle_t messageBufferHandle = xMessageBufferCreate(sizeof(lora_driver_payload_t));
  SemaphoreHandle_t breadConfigMutex = xSemaphoreCreateMutex();
  cloudDownlinkParams_t params = cloudDownlink_createParams(breadConfigMutex, messageBufferHandle, breadConfig);
  
  cloudDownlink_taskInit(params);
  cloudDownlink_taskRun();
  breadConfig_destroy(breadConfig);
}

TEST_F(DownlinkTestFixture, testPayloadLength1RecivedDoesntWriteToBreadConfig) {
  xMessageBufferReceive_fake.custom_fake = []( MessageBufferHandle_t xMessageBuffer,void *pvRxData,size_t xBufferLengthBytes,TickType_t xTicksToWait ) -> size_t {
    lora_driver_payload_t* payload = (lora_driver_payload_t*) pvRxData;
    (*payload).portNo = 1;
    (*payload).len = 1;
    (*payload).bytes[0] = 0b01100010;
    return (*payload).len;
  };

  breadConfig_t breadConfig = breadConfig_init();
  breadConfig->temperature = 420;
  MessageBufferHandle_t messageBufferHandle = xMessageBufferCreate(sizeof(lora_driver_payload_t));
  SemaphoreHandle_t breadConfigMutex = xSemaphoreCreateMutex();
  cloudDownlinkParams_t params = cloudDownlink_createParams(breadConfigMutex, messageBufferHandle, breadConfig);
  
  cloudDownlink_taskInit(params);
  cloudDownlink_taskRun();
  ASSERT_EQ(breadConfig->temperature, 420);
  breadConfig_destroy(breadConfig);
}

TEST_F(DownlinkTestFixture, testPayloadLength10RecivedDoesntWriteToBreadConfig) {
  xMessageBufferReceive_fake.custom_fake = []( MessageBufferHandle_t xMessageBuffer,void *pvRxData,size_t xBufferLengthBytes,TickType_t xTicksToWait ) -> size_t {
    lora_driver_payload_t* payload = (lora_driver_payload_t*) pvRxData;
    (*payload).portNo = 1;
    (*payload).len = 10;
    for (uint8_t i = 0; i < (*payload).len; i++) {
      (*payload).bytes[i] = 0b01100010;
    }
    return (*payload).len;
  };

  breadConfig_t breadConfig = breadConfig_init();
  breadConfig->temperature = 65535;
  MessageBufferHandle_t messageBufferHandle = xMessageBufferCreate(sizeof(lora_driver_payload_t));
  SemaphoreHandle_t breadConfigMutex = xSemaphoreCreateMutex();
  cloudDownlinkParams_t params = cloudDownlink_createParams(breadConfigMutex, messageBufferHandle, breadConfig);
  
  cloudDownlink_taskInit(params);
  cloudDownlink_taskRun();
  ASSERT_EQ(breadConfig->temperature, 65535);
  breadConfig_destroy(breadConfig);
}

TEST_F(DownlinkTestFixture, testPayloadLength3RecivedWritesTemperatureToBreadConfig) {
  xMessageBufferReceive_fake.custom_fake = []( MessageBufferHandle_t xMessageBuffer,void *pvRxData,size_t xBufferLengthBytes,TickType_t xTicksToWait ) -> size_t {
    lora_driver_payload_t* payload = (lora_driver_payload_t*) pvRxData;
    (*payload).portNo = 1;
    (*payload).len = 3;
    (*payload).bytes[0] = 0b01100010;
    (*payload).bytes[1] = 0b01111111;
    (*payload).bytes[2] = 0xff;
    return (*payload).len;
  };

  breadConfig_t breadConfig = breadConfig_init();
  breadConfig->temperature = 65535;
  MessageBufferHandle_t messageBufferHandle = xMessageBufferCreate(sizeof(lora_driver_payload_t));
  SemaphoreHandle_t breadConfigMutex = xSemaphoreCreateMutex();
  cloudDownlinkParams_t params = cloudDownlink_createParams(breadConfigMutex, messageBufferHandle, breadConfig);
  
  cloudDownlink_taskInit(params);
  cloudDownlink_taskRun();
  ASSERT_EQ(breadConfig->temperature, 354);
  breadConfig_destroy(breadConfig);
}

TEST_F(DownlinkTestFixture, testPayloadLength3RecivedWritesHumidityToBreadConfig) {
  xMessageBufferReceive_fake.custom_fake = []( MessageBufferHandle_t xMessageBuffer,void *pvRxData,size_t xBufferLengthBytes,TickType_t xTicksToWait ) -> size_t {
    lora_driver_payload_t* payload = (lora_driver_payload_t*) pvRxData;
    (*payload).portNo = 1;
    (*payload).len = 3;
    (*payload).bytes[0] = 0xff;
    (*payload).bytes[1] = 0b11001101;
    (*payload).bytes[2] = 0b10000000;
    return (*payload).len;
  };

  breadConfig_t breadConfig = breadConfig_init();
  breadConfig->humidity = 255;
  MessageBufferHandle_t messageBufferHandle = xMessageBufferCreate(sizeof(lora_driver_payload_t));
  SemaphoreHandle_t breadConfigMutex = xSemaphoreCreateMutex();
  cloudDownlinkParams_t params = cloudDownlink_createParams(breadConfigMutex, messageBufferHandle, breadConfig);
  
  cloudDownlink_taskInit(params);
  cloudDownlink_taskRun();
  ASSERT_EQ(breadConfig->humidity, 77);
  breadConfig_destroy(breadConfig);
}