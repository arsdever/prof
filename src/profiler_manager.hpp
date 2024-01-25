#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <stack>
#include <unordered_map>
#include <unordered_set>

#include "prof/data.hpp"
#include "prof/profiler_scope_keeper.hpp"

namespace prof
{

    class thread_local_profiler;

    class profiler_manager
    {
    public:
        /**
         * @brief Start profiling for the current function.
         *
         * This function is not supposed to be called directly. Use the @c profile function instead.
         *
         * @param function_name the name of the function to profile.
         * @return profiler_scope_keeper object to automatically pop the stack.
         */
        static profiler_scope_keeper start_profiling(std::string_view function_name);

        static profiler_scope_keeper start_frame(std::string_view function_name);

        /**
         * @brief Get the thread specific profiler for the given thread id.
         *
         * @param function_name the name of the function to profile.
         */
        static std::shared_ptr<thread_local_profiler> for_thread(std::string_view thread_id);

        static bool apply_frames(std::string_view thread_id, std::function<bool(const frame&)> e);

        /**
         * @brief Applies the operation to all of the data available for the given thread.
         *
         * @param thread_id the id of the thread to get the data for.
         * @param operation the operation to apply to the data.
         * @return true if the operation was executed for all of the data available, false otherwise.
         */
        static bool apply_data(std::string_view thread_id, std::function<bool(const data_sample&)> operation);

        static bool apply_frame_data(std::string_view                        thread_id,
                                     uint64_t                                frame_id,
                                     std::function<bool(const data_sample&)> operation);

        /**
         * @brief Get the vector of threads that have been profiled.
         *
         * @return std::vector<std::string> the vector of ids of threads that have been profiled.
         */
        static std::vector<std::string> known_threads();

    protected:
        static std::unordered_map<size_t, std::shared_ptr<thread_local_profiler>> _thread_profilers;
    };

} // namespace prof
