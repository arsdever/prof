cmake_minimum_required (VERSION 3.13)

set (CMAKE_CXX_STANDARD 20)

option (ENABLE_TESTING "Enable the testing of the project" ON)
option (BUILD_SAMPLES "Enable the testing of the project" ON)
project (profiler)

add_subdirectory (src)

if (BUILD_SAMPLES)
	add_subdirectory (samples)
endif (BUILD_SAMPLES)

if (ENABLE_TESTING)
    enable_testing ()
    add_subdirectory (test)
endif (ENABLE_TESTING)