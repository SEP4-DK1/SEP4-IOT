/*
* CMakeLists.txt
*
* Created: 26-04-2023 16:22:07
*  Author: Jesper Kristensen
*/
include_directories(${gtest_SOURCE_DIR}/ include ${gtest_SOURCE_DIR})

add_executable(runBasicTests
test1.cpp
)

target_link_libraries(runBasicTests gtest gtest_main)
target_link_libraries(runBasicTests FreeRTOS_defs)
target_link_libraries(runBasicTests Tasks)
target_link_libraries(runBasicTests DataModels)