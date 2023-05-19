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

    bool apply_for_data(std::string_view thread_id, std::function<bool(const data_sample&)> operation)
    {
        return profiler_manager::apply_for_data(thread_id, operation);
    }

} // namespace prof
