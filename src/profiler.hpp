#pragma once

#include <frame.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <scoped_profiler.hpp>
#include <sstream>
#include <stack>
#include <unordered_map>
#include <unordered_set>

namespace std
{

    inline string to_string(thread::id const& id)
    {
        stringstream ss;
        ss << id;
        return ss.str();
    }

} // namespace std

namespace prof
{

    class profiler_data_collector;
    class scoped_profiler;

    class profiler
    {
    public:
        using data_t = frame;
        template <typename T, typename U>
        using map_t = std::unordered_map<T, U>;
        template <typename T>
        using stack_t = std::stack<T>;
        template <typename T>
        using set_t  = std::unordered_set<T>;
        using name_t = std::string;
        using ptr_t  = std::unique_ptr<profiler>;

        static scoped_profiler              profile(std::string_view fname);
        static profiler&                    for_thread(std::string_view thid);
        static void                         set_implementation(std::unique_ptr<profiler>&& impl);
        static void                         dump_all_threads(std::ostream& s);
        void                                dump(std::ostream& s) const;
        void                                for_each_data(std::function<void(const data_t&)> e) const;
        static void                         for_each(std::function<void(const profiler&)> e);
        std::chrono::steady_clock::duration start_time() const;

        static void load(std::istream& buffer);
        static void save(std::ostream& buffer);

    protected:
        profiler(std::string_view thid) noexcept;
        void push_data(data_t const& d);
        void push_frame(std::string_view func);
        void pop_frame();

        friend class scoped_profiler;

        static map_t<name_t, ptr_t>         _profilers;
        static std::mutex                   _thd_mutex;
        stack_t<data_t>                     _data_stack;
        set_t<data_t>                       _data;
        std::mutex                          _mutex;
        name_t                              _id;
        std::chrono::steady_clock::duration _start_time;
    };

} // namespace prof