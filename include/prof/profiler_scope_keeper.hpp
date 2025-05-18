#pragma once

namespace prof
{

    struct base_profiler;

    /**
     * @brief A RAII class to automatically finish a profiling.
     *
     * @see thread_local_profiler
     * @see profiler
     *
     * @ingroup Profiler
     */
    class profiler_scope_keeper
    {
    public:
        /**
         * @brief Constructor.
         *
         * @param profiler the profiler to finish.
         */
        profiler_scope_keeper(base_profiler& profiler);

        /**
         * @brief Destructor.
         *
         * Finishes the profiling.
         */
        ~profiler_scope_keeper();

        /**
         * @brief Move constructor.
         *
         * Used to allow profiling a scope without a need for explicit scopes. This is helpful when there is a
         * instantiation happening, which needs separate profiling.
         * In other words, this is a convenience function to finish the @c other scope and start a new one.
         *
         * @param other the profiler to move.
         *
         * @return the moved profiler.
         */
        profiler_scope_keeper& operator=(profiler_scope_keeper&& other) noexcept;

        /**
         * @brief Finish the profiling of the scope.
         */
        void finish();

    private:
        profiler_scope_keeper& operator=(const profiler_scope_keeper& other) = delete;

    private:
        base_profiler* _ref;
    };

} // namespace prof
