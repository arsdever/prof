#pragma once

namespace prof
{

    /**
     * @brief An interface for any profiler.
     */
    struct base_profiler
    {
        /**
         * @brief Destructor.
         */
        virtual ~base_profiler() = default;

        /**
         * @brief Finishes the profiling.
         */
        virtual void finish() = 0;
    };

} // namespace prof
