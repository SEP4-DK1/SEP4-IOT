/*
 * CMakeLists.txt
 *
 * Created: 26-04-2023 16:21:46
 *  Author: Jesper Kristensen
 */
project(test)

add_subdirectory(lib/googletest-1.13.0)
add_subdirectory(basicTest)

set(SOURCES_TEST
main.cpp
FreeRTOS_defs/FreeRTOS_FFF_MocksDefinitions.cpp
../app/src/task1.c
${SRCS}
)

# This makes the test.cpp files able to #include target libraries without errors.
include_directories(
FreeRTOS_defs  # Mock and FreeRTOS definitions
../app/include
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