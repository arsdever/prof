#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <stack>
#include <unordered_map>
#include <unordered_set>

#include "prof/frame.hpp"
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

        /**
         * @brief Get the thread specific profiler for the given thread id.
         *
         * @param function_name the name of the function to profile.
         */
        static std::shared_ptr<thread_local_profiler> for_thread(std::string_view thread_id);

        /**
         * @brief Applies the operation to all of the data available for the given thread.
         *
         * @param thread_id the id of the thread to get the data for.
         * @param operation the operation to apply to the data.
         * @return true if the operation was executed for all of the data available, false otherwise.
         */
        static bool apply_for_data(std::string_view thread_id, std::function<bool(const frame&)> operation);

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
