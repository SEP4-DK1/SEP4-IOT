/*
 * CMakeLists.txt
 *
 * Created: 26-04-2023 16:20:18
 *  Author: Jesper Kristensen
 */

cmake_minimum_required(3.16)

project(SEP4-IOT)

add_definitions(-std=gnu++Ox)

set (SOURCE_FILES main.cpp)
add_executable(SEP4-IOT-run ${SOURCE_FILES})

include_directories(SEP4-IOT)

add_subdirectory(SEP4-IOT)
add_subdirectory(test)

target_link_libraries(SEP4-IOT-run SEP4-IOT)