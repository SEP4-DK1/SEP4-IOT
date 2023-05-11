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

include_directories(Tasks)
include_directories(DataModels)

add_subdirectory(Tasks)
add_subdirectory(test)
add_subdirectory(DataModels)

target_link_libraries(Tasks-run Tasks)
target_link_libraries(DataModels-run DataModels)