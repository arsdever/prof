add_library (profiler STATIC
	${CMAKE_CURRENT_SOURCE_DIR}/profiler.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/profiler.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/frame.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/frame.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/scoped_profiler.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/scoped_profiler.hpp
)
target_include_directories (profiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})