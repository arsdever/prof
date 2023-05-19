#pragma once

#include <chrono>
#include <iostream>
#include <string>

namespace prof
{

    class data_sample
    {
    public:
        using depth_t = uint64_t;
        using id_t    = uint64_t;

    public:
        data_sample();
        data_sample(std::string const& name, depth_t depth);
        void                          stop();
        std::chrono::duration<double> diff() const;

        uint64_t                              id() const;
        std::string                           name() const;
        depth_t                               depth() const;
        std::chrono::steady_clock::time_point start() const;
        std::chrono::steady_clock::time_point end() const;

        bool operator==(data_sample const& o) const;

        static void        save(std::ostream& o, data_sample const& f);
        static data_sample load(std::istream& i);

    private:
        std::string                           _name;
        depth_t                               _depth;
        std::chrono::steady_clock::time_point _start;
        std::chrono::steady_clock::time_point _stop;
        id_t                                  _id;

        static id_t _id_counter;
    };

    class frame
    {
    public:
        using id_t = uint64_t;

    public:
        frame();

        void add(data_sample sample);
        std::vector<data_sample> const& samples() const;

    private:
        std::vector<data_sample> _frame_data;
        id_t                     _id;

        static id_t _id_counter;
    };

} // namespace prof

namespace std
{
    template <>
    struct hash<prof::data_sample>
    {
        std::size_t operator()(prof::data_sample const& o) const
        {
            return std::hash<uint64_t> {}(o.id()) ^ std::hash<std::string_view> {}(o.name()) ^
                   std::hash<prof::data_sample::depth_t> {}(o.depth()) ^
                   std::hash<long long> {}(o.start().time_since_epoch().count());
        }
    };
} // namespace std
