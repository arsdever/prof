#include <iostream>
#include <profiler.hpp>
#include <profiler_data_collector.hpp>
#include <scoped_profiler.hpp>

class unit
{
public:
    unit()
    {
        // _profiler.register_function(0, "long_running_task");
        // _profiler.register_function(1, "slow_motion");
    }

    void slow_motion()
    {
        auto p = prof::profiler::profile(__func__);
        static int a[] { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        while (std::ranges::next_permutation(a).found) { } // generates 12! permutations
    }

    std::string long_running_task(int n = 10)
    {
        auto p = prof::profiler::profile(__func__);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (n)
            long_running_task(n - 1);

        return "Hello";
    }

    long long_running_task_2branch(int n = 10)
    {
        auto p = prof::profiler::profile(__func__);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (!n || n == 1)
            {
                return 1;
            }

        return long_running_task_2branch(n - 1) + long_running_task_2branch(n - 2);
    }

public:
    prof::profiler _profiler;
};

int main(int argc, char** argv)
{
    prof::profiler::set_implementation(std::move(std::make_unique<prof::profiler>()));
    unit u {};
    std::cout << u.long_running_task() << std::endl;
    std::cout << u.long_running_task_2branch() << std::endl;
    u.slow_motion();
    prof::profiler::implementation().dump(std::cout);
    return 0;
}