add_library(profiler_interface INTERFACE prof/profiler.hpp prof/frame.hpp
            prof/profiler_scope_keeper.hpp)
add_library(profiler::interface ALIAS profiler_interface)

target_include_directories(profiler_interface
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
