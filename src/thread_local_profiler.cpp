#include "thread_local_profiler.hpp"

#include "prof/profiler_scope_keeper.hpp"

namespace prof
{

    frame_keeper::frame_keeper(thread_local_profiler& ref, std::string function_name)
        : _ref(ref)
        , _function_name(std::move(function_name))
    {
    }

    void frame_keeper::finish() { _ref.frame_pop(); }

    std::string thread_local_profiler::id() const { return _id; }

    profiler_scope_keeper thread_local_profiler::stack_push(std::string_view function_name)
    {
        _stack.emplace(std::string { function_name }, _stack.size());
        return profiler_scope_keeper { *this };
    }

    profiler_scope_keeper thread_local_profiler::frame_push(std::string_view function_name)
    {
        _frame_keeper = std::make_unique<frame_keeper>(*this, std::string { function_name });
        _stack.emplace(std::string { function_name }, _stack.size());
        if (_frames.size() > 10)
            {
                _frames.erase(_frames.begin());
            }
        _frames.emplace_back();
        return profiler_scope_keeper { *_frame_keeper };
    }

    void thread_local_profiler::stack_pop()
    {
        if (_frames.empty())
            {
                _frames.emplace_back();
            }

        if (!_stack.empty())
            {
                _stack.top().stop();
                _frames.back().add(std::move(_stack.top()));
                _stack.pop();
            }
    }

    void thread_local_profiler::frame_pop()
    {
        while (!_stack.empty())
            {
                _stack.top().stop();
                _frames.back().add(std::move(_stack.top()));
                _stack.pop();
            }
    }

    void thread_local_profiler::finish() { stack_pop(); }

    bool thread_local_profiler::for_each_data(std::function<bool(const data_sample&)> operation) const
    {
        for (int i = 0; i < _frames.back().samples().size(); ++i)
            {
                data_sample const& f = _frames.back().samples()[ i ];
                if (!operation(f))
                    return false;
            }

        return true;
    }

} // namespace prof
