#include "frame.hpp"

using namespace std::chrono;

namespace prof
{
    frame::frame(std::string const& name, depth_t depth)
        : _name { name }
        , _depth { depth }
        , _start { steady_clock::now() }
    {
    }

    void frame::stop() { _stop = steady_clock::now(); }

    duration<double> frame::diff() const { return duration<double>(_stop - _start); }

    std::string_view frame::name() const { return _name; }

    frame::depth_t frame::depth() const { return _depth; }

    steady_clock::time_point frame::start() const { return _start; }

    steady_clock::time_point frame::end() const { return _stop; }

    bool frame::operator==(frame const& o) const { return std::hash<frame> {}(*this) == std::hash<frame> {}(o); }

    void frame::save(std::ostream& o, frame const& f)
    {
        o << f._name << ' ' << f._depth << ' ' << duration_cast<duration<double>>(f._start.time_since_epoch()).count()
          << ' ' << duration_cast<duration<double>>(f._stop.time_since_epoch()).count() << ' ';
    }

    frame frame::load(std::istream& i)
    {
        std::string name;
        depth_t     depth;
        double      start;
        double      end;
        i >> name >> depth >> start >> end;
        frame result { name, depth };
        result._start = steady_clock::time_point { duration_cast<steady_clock::duration>(duration<double> { start }) };
        result._stop  = steady_clock::time_point { duration_cast<steady_clock::duration>(duration<double> { end }) };
        return result;
    }
} // namespace prof