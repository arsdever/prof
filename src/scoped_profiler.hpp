#pragma once

#include <string_view>

namespace prof
{

    class profiler;

    class scoped_profiler
    {
    public:
        scoped_profiler(profiler& p, std::string_view fname);
        scoped_profiler(const scoped_profiler&) = delete;
        scoped_profiler(scoped_profiler&&)      = delete;
        ~scoped_profiler();

    private:
        profiler& _ref;
    };

} // namespace prof