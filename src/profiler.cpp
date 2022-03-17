#include "profiler.hpp"

#include "profiler_data_collector.hpp"
#include "scoped_profiler.hpp"

namespace prof
{
    namespace
    {
        std::mutex profiler_mutex;
    }

    scoped_profiler profiler::profile(std::string_view fname, std::string const& thid)
    {
        return { implementation(), fname };
    }

    std::shared_ptr<profiler_data_collector> profiler::get_collector(std::string_view thid)
    {
        std::lock_guard<std::mutex> guard { _mutex };
        std::string                 key { thid };
        auto                        it { _collectors.find(key) };
        if (it == _collectors.end())
            {
                it =
                    _collectors.insert(std::make_pair(key, std::make_shared<profiler_data_collector>(implementation())))
                        .first;
            }

        return it->second;
    }

    // std::shared_ptr<profiler_data_collector> profiler::get_data_collector(std::string_view thid)
    //{
    //     return implementation().get_collector(thid);
    // }

    void profiler::dump(std::ostream& s)
    {
        std::for_each(_data.begin(), _data.end(), [ &s ](data_t const& f) {
            s << f.name() << " : " << f.depth() << " : " << f.diff() << std::endl;
        });
    }

    profiler& profiler::implementation()
    {
        std::lock_guard<std::mutex> guard { profiler_mutex };
        return *_impl;
    }

    void profiler::set_implementation(std::unique_ptr<profiler>&& impl)
    {
        std::lock_guard<std::mutex> guard { profiler_mutex };
        _impl = std::move(impl);
    }

    void profiler::push_data(data_t const& d) { _data.insert(d); }

    void profiler::push_frame(std::string_view func) { _data_stack.push({ std::string { func }, _data_stack.size() }); }

    void profiler::pop_frame()
    {
        _data_stack.top().stop();
        _data.insert(std::move(_data_stack.top()));
        _data_stack.pop();
    }

    profiler::data_t::depth_t profiler::current_depth() const { return _data.size(); }

    std::unique_ptr<profiler> profiler::_impl {};

} // namespace prof