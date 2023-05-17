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
    private:
        /**
         * @brief Copy constructor.
         *
         * Deleted since the profiler is not copyable.
         */
        profiler_scope_keeper(const profiler_scope_keeper&) = delete;

        /**
         * @brief Copy assignment operator.
         *
         * Deleted since the profiler is not copyable.
         */
        profiler_scope_keeper(profiler_scope_keeper&&) = delete;

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

    private:
        base_profiler& _ref;
    };

} // namespace prof
