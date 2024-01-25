#include "prof/profiler.hpp"

#include "prof/profiler_scope_keeper.hpp"
#include "profiler_manager.hpp"

namespace prof
{

    profiler_scope_keeper profile(std::string_view function_name)
    {
        return profiler_manager::start_profiling(function_name);
    }

    profiler_scope_keeper profile_frame(std::string_view function_name)
    {
        return profiler_manager::start_frame(function_name);
    }

    std::vector<std::string> known_threads() { return profiler_manager::known_threads(); }

    bool apply_frames(std::string_view thread_id, std::function<bool(const frame&)> operation)
    {
        return profiler_manager::apply_frames(thread_id, operation);
    }

    bool apply_data(std::string_view thread_id, std::function<bool(const data_sample&)> operation)
    {
        return profiler_manager::apply_data(thread_id, operation);
    }

    bool
    apply_frame_data(std::string_view thread_id, uint64_t frame_id, std::function<bool(const data_sample&)> operation)
    {
        return profiler_manager::apply_frame_data(thread_id, frame_id, operation);
    }

} // namespace prof
