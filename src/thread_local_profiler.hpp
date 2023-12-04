#pragma once

#include <functional>
#include <memory>
#include <deque>
#include <stack>
#include <string>
#include <string_view>

#include "base_profiler.hpp"
#include "prof/data.hpp"

namespace prof
{

    class thread_local_profiler;

    struct frame_keeper : base_profiler
    {
        thread_local_profiler& _ref;
        std::string            _function_name;
        frame_keeper(thread_local_profiler& ref, std::string function_name);

        void finish() override;
    };

    class profiler_scope_keeper;

    /**
     * @brief Class for profiler for a single thread.
     *
     * This class is used to profile a single thread. It is not thread-safe. Each thread should have its own specific
     * instance of this, though not only one.
     *
     * Usually, the user should not use this class directly. Instead, the user should use the @c profile function. It
     * will automatically find the correct profiler for the current thread, push a new frame into its stack and return a
     * @c profiler_scope_keeper object to automatically pop the stack.
     *
     * @see profiler
     * @see profiler_manager
     *
     * @ingroup Profiler
     */
    class thread_local_profiler : base_profiler
    {
        /**
         * @brief Copy constructor.
         *
         * Deleted since the profiler is not copyable.
         */
        thread_local_profiler(const thread_local_profiler&) = delete;
        /**
         * @brief Copy assignment operator.
         *
         * Deleted since the profiler is not copyable.
         */
        thread_local_profiler& operator=(const thread_local_profiler&) = delete;

    public:
        /**
         * @brief Constructor.
         *
         * @param id The id of the thread.
         */
        thread_local_profiler(std::string_view id) noexcept
            : _id { id }
        {
        }

        /**
         * @brief Move constructor.
         */
        thread_local_profiler(thread_local_profiler&&) noexcept = default;

        /**
         * @brief Get the id of the thread the profiler is attached to.
         *
         * @returns the id of the thread the profiler is attached to.
         */
        std::string id() const;

        /**
         * @brief Pushes a new stack frame into the profiler.
         *
         * This function is used to push a new stack frame into the profiler. Calling this function returns a @c
         * profiler_scope_keeper object which should be kept until the end of the scope. When the @c
         * profiler_scope_keeper object is destroyed, the stack frame is popped from the profiler.
         *
         * @param function_name the name of the function the thread runs into.
         * @returns a @c profiler_scope_keeper object to automatically pop the stack.
         */
        profiler_scope_keeper stack_push(std::string_view function_name);

        /**
         * @brief Starts profiling of a new frame.
         *
         * @param function_name the name of the function the thread runs into.
         * @return a @c profiler_scope_keeper the scope keeper object.
         */
        profiler_scope_keeper frame_push(std::string_view function_name);

        /**
         * @brief Pops the last stack frame from the profiler.
         * This function is used to pop the last stack frame from the profiler. It is called automatically by the @c
         * profiler_scope_keeper object returned by the @c stack_push function.
         */
        void stack_pop();

        /**
         * @brief Finishes the current frame.
         */
        void frame_pop();

        /**
         * @brief Executes a function for each frame in the profiler.
         *
         * This function is used to execute a function for each frame in the profiler. The function is executed in the
         * order of the frames in the stack, from the top to the bottom. The operation performed should return true if
         * the loop should continue, false otherwise.
         *
         * @param operation to perform for each frame.
         * @returns true if the operation was executed for all of the frames, false otherwise.
         */
        bool for_each_data(std::function<bool(const data_sample&)> operation) const;

#pragma region base_profiler

    public:
        /**
         * @brief Finish the scope of the profiler.
         */
        void finish() override;

#pragma endregion

        std::string             _id;
        std::stack<data_sample> _stack;
        std::deque<frame>      _frames;
        friend struct frame_keeper;
        std::unique_ptr<frame_keeper> _frame_keeper;
    };

} // namespace prof

namespace std
{

    template <>
    struct hash<::prof::thread_local_profiler>
    {
        size_t operator()(prof::thread_local_profiler const& p) const noexcept
        {
            return std::hash<std::string> {}(p._id);
        }
    };

} // namespace std
