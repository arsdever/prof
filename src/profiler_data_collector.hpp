#pragma once

#include <stack>

#include <frame.hpp>

namespace prof
{
    class profiler;

    class profiler_data_collector
    {
    public:
        using frame_t = frame;

    public:
        profiler_data_collector(profiler& p);

        void push_frame(std::string_view fname);
        void pop_frame();

    private:
        profiler&           _profiler;
        std::stack<frame_t> _frames;
    };
} // namespace prof