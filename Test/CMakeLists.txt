cmake_minimum_required(VERSION 3.14)
set(projectName SEP4IOTTests)
project(${projectName})

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)

set(DOWNLOAD_EXTRACT_TIMESTAMP true)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest )

set(SOURCES_TEST
    SensorDataTests.cpp
    temperatureTransmitTest.cpp
    FreeRTOS_defs/FreeRTOS_FFF_MocksDefinitions.cpp
    ../SEP4-IOT/Tasks/DataCollection.c
    ../SEP4-IOT/Tasks/TemperatureTransmit.c
    ../SEP4-IOT/DataModels/SensorData.c
    ${SRCS}
)

# This makes the test.cpp files able to #include target libraries without errors.  
include_directories(
  FreeRTOS_defs  # Mock and FreeRTOS definitions
  ../SEP4-IOT/Tasks
  ../SEP4-IOT/DataModels
  ../SEP4-IOT/drivers
  ../SEP4-IOT/Util
)

# Something to 
enable_testing()

add_executable(
  ${projectName}
  ${SOURCES_TEST}
)
target_link_libraries(
  ${projectName}
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(${projectName})
