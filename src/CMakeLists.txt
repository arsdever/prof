add_library(
  profiler_lib STATIC
  profiler.cpp
  data.cpp
  profiler_scope_keeper.cpp
  thread_local_profiler.cpp
  thread_local_profiler.hpp
  profiler_manager.cpp
  profiler_manager.hpp)
add_library(profiler::lib ALIAS profiler_lib)

target_link_libraries(profiler_lib PUBLIC profiler::interface)

if(PROF_OPENGL)
  target_sources(profiler_lib PUBLIC opengl/drawer.cpp opengl/shader.hpp)
  target_link_libraries(profiler_lib PUBLIC GLAD)
endif()
