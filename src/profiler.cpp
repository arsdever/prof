#include <iomanip>

#include "profiler.hpp"

#include "scoped_profiler.hpp"

using namespace std::chrono;

namespace prof
{
    namespace
    {
        std::mutex profiler_mutex;
    }

    scoped_profiler profiler::profile(std::string_view fname)
    {
        std::string thid = std::to_string(std::this_thread::get_id());
        return { for_thread(std::move(thid)), fname };
    }

    profiler& profiler::for_thread(std::string_view thid)
    {
        std::lock_guard<std::mutex> guard { _thd_mutex };
        auto                        it = _profilers.find(name_t { thid });
        if (it == _profilers.end())
            {
                it = _profilers
                         .emplace(std::make_pair(name_t { thid }, std::unique_ptr<profiler> { new profiler { thid } }))
                         .first;
                it->second->_start_time = steady_clock::now().time_since_epoch();
            }

        return *it->second.get();
    }

    profiler::profiler(std::string_view thid) noexcept
        : _id { thid }
    {
        _data.resize(_max_size);
    }

    void profiler::dump(std::ostream& s) const
    {
        std::for_each(_data.begin(), _data.end(), [ id = _id, &s ](data_t const& f) {
            s << "[ thread " << id << "] " << f.name() << " : " << f.depth() << " : " << f.diff() << std::endl;
        });
    }

    void profiler::dump_all_threads(std::ostream& s)
    {
        std::for_each(_profilers.begin(), _profilers.end(), [ &s ](auto const& p) { p.second->dump(s); });
    }

    void profiler::for_each_data(std::function<void(const data_t&)> e) const
    {
        std::for_each(_data.begin(), _data.end(), [ e ](data_t const& f) { e(f); });
    }

    void profiler::for_each(std::function<void(const profiler&)> e)
    {
        std::for_each(_profilers.begin(), _profilers.end(), [ e ](auto const& p) { e(*p.second); });
    }

    steady_clock::duration profiler::start_time() const { return _start_time; }

    void profiler::push_data(data_t const& d)
    {
        if (_current_data_index >= _max_size)
            {
                _current_data_index = 0;
            }

        _data[ _current_data_index++ ] = d;
    }

    void profiler::push_frame(std::string_view func) { _data_stack.push({ std::string { func }, _data_stack.size() }); }

    void profiler::pop_frame()
    {
        _data_stack.top().stop();
        push_data(_data_stack.top());
        _data_stack.pop();
    }

    std::unordered_map<profiler::name_t, std::unique_ptr<profiler>> profiler::_profilers;

    std::mutex profiler::_thd_mutex;

    void profiler::load(std::istream& in)
    {
        std::lock_guard<std::mutex> guard { _thd_mutex };
        size_t                      count;
        in >> count;
        while (count--)
            {
                std::string name;
                size_t      data_count;
                in >> name >> data_count;
                std::unique_ptr<profiler> p { new profiler { name } };
                while (data_count--)
                    {
                        p->push_data(data_t::load(in));
                    }
                double dur;
                in >> dur;
                p->_start_time = duration_cast<steady_clock::duration>(duration<double> { dur });
                _profilers.emplace(std::make_pair(p->_id, std::move(p)));
            }
    }

    void profiler::save(std::ostream& out)
    {
        std::lock_guard<std::mutex> guard { _thd_mutex };
        out << std::setprecision(50) << _profilers.size() << ' ';
        for (auto const& p : _profilers)
            {
                out << p.second->_id << ' ' << p.second->_data.size() << ' ';
                for (auto const& d : p.second->_data)
                    {
                        data_t::save(out, d);
                    }
                out << duration_cast<duration<double>>(p.second->_start_time).count() << ' ';
            }
    }

} // namespace prof