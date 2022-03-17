#include "scoped_profiler.hpp"

#include "profiler.hpp"

namespace prof
{

    scoped_profiler::scoped_profiler(profiler& p, std::string_view fname)
        : _ref(p)
    {
        _ref.push_frame(fname);
    }

    scoped_profiler::~scoped_profiler() { _ref.pop_frame(); }

} // namespace prof