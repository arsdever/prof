#pragma once

#include <iostream>
#include <chrono>
#include <string>

namespace prof
{
    class frame
    {
    public:
        using depth_t = uint64_t;

    public:
        frame(std::string const& name, depth_t depth);
        void                          stop();
        std::chrono::duration<double> diff() const;

        std::string                           name() const;
        depth_t                               depth() const;
        std::chrono::steady_clock::time_point start() const;
        std::chrono::steady_clock::time_point end() const;

        bool operator==(frame const& o) const;

        static void save(std::ostream& o, frame const& f);
        static frame load(std::istream& i);

    private:
        std::string                           _name;
        depth_t                               _depth;
        std::chrono::steady_clock::time_point _start;
        std::chrono::steady_clock::time_point _stop;
    };
} // namespace prof

namespace std
{
    template <>
    struct hash<prof::frame>
    {
        std::size_t operator()(prof::frame const& o) const
        {
            return std::hash<std::string_view> {}(o.name()) ^ std::hash<prof::frame::depth_t> {}(o.depth()) ^
                   std::hash<long long> {}(o.start().time_since_epoch().count());
        }
    };
} // namespace std
