#include "frame.hpp"

namespace prof
{
    frame::frame(std::string const& name, depth_t depth)
        : _name { name }
        , _depth { depth }
        , _start { std::chrono::steady_clock::now() }
    {
    }

    void frame::stop() { _stop = std::chrono::steady_clock::now(); }

    std::chrono::duration<double> frame::diff() const { return std::chrono::duration<double>(_stop - _start); }

    std::string_view frame::name() const { return _name; }

    frame::depth_t frame::depth() const { return _depth; }

    std::chrono::steady_clock::time_point frame::start() const { return _start; }

    std::chrono::steady_clock::time_point frame::end() const { return _stop; }

    bool frame::operator==(frame const& o) const { return std::hash<frame> {}(*this) == std::hash<frame> {}(o);
    }
} // namespace prof