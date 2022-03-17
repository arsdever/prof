include_directories (${CMAKE_SOURCE_DIR}/src)

add_executable (sample1 sample1.cpp)
add_executable (sample2 sample2.cpp)
add_executable (sample3
	sample3.cpp
	${CMAKE_SOURCE_DIR}/src/profiler.cpp
	${CMAKE_SOURCE_DIR}/src/profiler.hpp
	${CMAKE_SOURCE_DIR}/src/profiler_data_collector.cpp
	${CMAKE_SOURCE_DIR}/src/profiler_data_collector.hpp
	${CMAKE_SOURCE_DIR}/src/frame.cpp
	${CMAKE_SOURCE_DIR}/src/frame.hpp
	${CMAKE_SOURCE_DIR}/src/scoped_profiler.cpp
	${CMAKE_SOURCE_DIR}/src/scoped_profiler.hpp
)