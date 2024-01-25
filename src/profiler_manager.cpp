#include <thread>

#include "profiler_manager.hpp"

#include "thread_local_profiler.hpp"

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

    namespace
    {
        std::mutex thread_specific_profiler_mutex;
    }

    profiler_scope_keeper profiler_manager::start_profiling(std::string_view function_name)
    {
        auto thread_profiler = for_thread(std::to_string(std::this_thread::get_id()));
        return thread_profiler->stack_push(function_name);
    }

    profiler_scope_keeper profiler_manager::start_frame(std::string_view function_name)
    {
        auto thread_profiler = for_thread(std::to_string(std::this_thread::get_id()));
        return thread_profiler->frame_push(function_name);
    }

    std::shared_ptr<thread_local_profiler> profiler_manager::for_thread(std::string_view thread_id)
    {
        std::lock_guard<std::mutex> lock { thread_specific_profiler_mutex };
        // lookup for the profiler
        // if not found, create a new one
        // otherwise return the existing one
        if (_thread_profilers.find(std::hash<std::string_view> {}(thread_id)) == _thread_profilers.end())
            {
                _thread_profilers.emplace(std::hash<std::string_view> {}(thread_id),
                                          std::make_shared<thread_local_profiler>(thread_id));
            }

        return _thread_profilers[ std::hash<std::string_view> {}(thread_id) ];
    }

    bool profiler_manager::apply_frames(std::string_view thread_name, std::function<bool(const frame&)> e)
    {
        auto thread_profiler = for_thread(thread_name);
        return thread_profiler->for_each_frame(e);
    }

    bool profiler_manager::apply_data(std::string_view thread_name, std::function<bool(const data_sample&)> e)
    {
        auto thread_profiler = for_thread(thread_name);
        return thread_profiler->for_each_data(e);
    }

    bool profiler_manager::apply_frame_data(std::string_view                        thread_name,
                                            uint64_t                                frame_id,
                                            std::function<bool(const data_sample&)> e)
    {
        auto thread_profiler = for_thread(thread_name);
        return thread_profiler->for_each_data_frame(frame_id, e);
    }

    std::vector<std::string> profiler_manager::known_threads()
    {
        std::vector<std::string> result;
        for (auto& [ _, profiler ] : _thread_profilers)
            {
                result.push_back(profiler->id());
            }

        return result;
    }

    std::unordered_map<size_t, std::shared_ptr<thread_local_profiler>> profiler_manager::_thread_profilers;

} // namespace prof
