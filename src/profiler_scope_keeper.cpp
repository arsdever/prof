#include "prof/profiler_scope_keeper.hpp"

#include "base_profiler.hpp"

namespace prof
{

    profiler_scope_keeper::profiler_scope_keeper(base_profiler& p)
        : _ref(p)
    {
    }

    profiler_scope_keeper::~profiler_scope_keeper() { _ref.finish(); }

} // namespace prof
