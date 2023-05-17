#include "thread_local_profiler.hpp"

#include "prof/profiler_scope_keeper.hpp"

namespace prof
{

    std::string thread_local_profiler::id() const { return _id; }

    profiler_scope_keeper thread_local_profiler::stack_push(std::string_view func)
    {
        _stack.push({ std::string { func }, _stack.size() });
        return profiler_scope_keeper { *this };
    }

    void thread_local_profiler::stack_pop()
    {
        if (!_stack.empty())
            {
                _stack.top().stop();
                _records.push_back(std::move(_stack.top()));
                _stack.pop();
            }
    }

    void thread_local_profiler::finish() { stack_pop(); }

    bool thread_local_profiler::for_each_data(std::function<bool(const frame&)> operation) const
    {
        for (int i = 0; i < _records.size(); ++i)
            {
                frame const& f = _records[ i ];
                if (!operation(f))
                    return false;
            }

        return true;
    }

} // namespace prof
