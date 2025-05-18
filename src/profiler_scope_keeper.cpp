#include "prof/profiler_scope_keeper.hpp"

#include "base_profiler.hpp"

namespace prof
{

    profiler_scope_keeper::profiler_scope_keeper(base_profiler& p)
        : _ref(&p)
    {
    }

    profiler_scope_keeper::~profiler_scope_keeper() { finish(); }

    profiler_scope_keeper& profiler_scope_keeper::operator=(profiler_scope_keeper&& o) noexcept
    {
        finish();
        _ref   = o._ref;
        o._ref = nullptr;
        return *this;
    }

    void profiler_scope_keeper::finish()
    {
        if (_ref)
            {
                _ref->finish();
                _ref = nullptr;
            }
    }

} // namespace prof
