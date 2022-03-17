#include "profiler_data_collector.hpp"

#include "profiler.hpp"

namespace prof
{
    profiler_data_collector::profiler_data_collector(profiler& p)
        : _profiler { p }
    {
    }

    void profiler_data_collector::push_frame(std::string_view fname)
    {
        _frames.push({ std::string { fname }, _frames.size() });
    }

    void profiler_data_collector::pop_frame()
    {
        _frames.top().stop();
        _profiler.push_data(_frames.top());
        _frames.pop();
    }
} // namespace prof