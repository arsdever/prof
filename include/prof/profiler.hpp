#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <vector>

#include "prof/data.hpp"
#include "prof/profiler_scope_keeper.hpp"

namespace prof
{
    /**
     * @brief Starts a profiling of the current stack
     *
     * This is the main function of the library. It can be used from anywhere in the code to profile the current stack.
     * It will automatically find the correct profiler for the current thread, push a new frame into its stack and
     * return a @c profiler_scope_keeper object to automatically pop the stack. The function name is used to identify
     * the frame in the profiler. It is recommended to use the @c __FUNCTION__ macro to automatically get the name of
     * the function. The @c __PRETTY_FUNCTION__ macro can also be used to get the full signature of the function. The
     * @c __func__ macro can also be used to get the name of the function, but it is not recommended since it is not
     * standardized and may not be available on all compilers.
     *
     * @param function_name the name of the function to profile.
     * @return profiler_scope_keeper object to automatically pop the stack.
     *
     * @see profiler_scope_keeper
     * @see thread_local_profiler
     * @see profiler_manager
     *
     * @ingroup Profiler
     */
    extern profiler_scope_keeper profile(std::string_view function_name);

    extern profiler_scope_keeper profile_frame(std::string_view function_name);

    /**
     * @brief Get the vector of threads that have been profiled.
     *
     * The ids of the threads can later be used to fetch the profiling data for the given thread.
     *
     * @return std::vector<std::string> the vector of ids of threads that have been profiled.
     */
    extern std::vector<std::string> known_threads();

    /**
     * @brief Applies the operation to all of the data available for the given thread.
     *
     * The operation is applied to all of the frames that have been profiled for the given thread. The operation is
     * applied in the order of the frames, from the oldest to the newest. The operation can return false to stop the
     * iteration.
     *
     * @param thread_id the id of the thread to get the data for.
     * @param operation the operation to apply to the data.
     * @return true if the operation was executed for all of the data available, false otherwise.
     */
    extern bool apply_for_data(std::string_view thread_id, std::function<bool(const data_sample&)> operation);

} // namespace prof
