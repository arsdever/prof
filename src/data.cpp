#include "prof/data.hpp"

using namespace std::chrono;

namespace prof
{
    data_sample::data_sample()
        : _name {}
        , _depth {}
        , _start {}
        , _id { _id_counter++ }
    {
    }

    data_sample::data_sample(std::string const& name, depth_t depth)
        : _name { name }
        , _depth { depth }
        , _start { steady_clock::now() }
        , _id { _id_counter++ }
    {
    }

    void data_sample::stop() { _stop = steady_clock::now(); }

    duration<double> data_sample::diff() const { return duration<double>(_stop - _start); }

    uint64_t data_sample::id() const { return _id; }

    std::string data_sample::name() const { return _name; }

    data_sample::depth_t data_sample::depth() const { return _depth; }

    steady_clock::time_point data_sample::start() const { return _start; }

    steady_clock::time_point data_sample::end() const { return _stop; }

    bool data_sample::operator==(data_sample const& o) const
    {
        return std::hash<data_sample> {}(*this) == std::hash<data_sample> {}(o);
    }

    void data_sample::save(std::ostream& o, data_sample const& f)
    {
        o << f._name << ' ' << f._depth << ' ' << duration_cast<duration<double>>(f._start.time_since_epoch()).count()
          << ' ' << duration_cast<duration<double>>(f._stop.time_since_epoch()).count() << ' ';
    }

    data_sample data_sample::load(std::istream& i)
    {
        std::string name;
        depth_t     depth;
        double      start;
        double      end;
        i >> name >> depth >> start >> end;
        data_sample result { name, depth };
        result._start = steady_clock::time_point { duration_cast<steady_clock::duration>(duration<double> { start }) };
        result._stop  = steady_clock::time_point { duration_cast<steady_clock::duration>(duration<double> { end }) };
        return result;
    }

    data_sample::id_t data_sample::_id_counter = 0;

    frame::frame()
        : _frame_data {}
        , _id { _id_counter++ }
    {
    }

    void frame::add(data_sample sample) { _frame_data.push_back(std::move(sample)); }

    std::vector<data_sample> const& frame::samples() const { return _frame_data; }

    frame::id_t frame::_id_counter = 0;

} // namespace prof
