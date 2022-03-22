#include <fstream>
#include <iostream>
#include <profiler.hpp>
#include <scoped_profiler.hpp>

class unit
{
public:
    unit() { }

    void slow_motion()
    {
        auto       p = prof::profiler::profile(__func__);
        static int a[] { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        while (std::ranges::next_permutation(a).found) { }
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
};

int main(int argc, char** argv)
{
    unit u {};
    std::cout << u.long_running_task() << std::endl;
    u.slow_motion();
    std::thread t1 { [ &u ]() { std::cout << u.long_running_task_2branch() << std::endl; } };
    std::thread t2 { [ &u ]() { std::cout << u.long_running_task_2branch() << std::endl; } };
    std::thread t3 { [ &u ]() { std::cout << u.long_running_task_2branch() << std::endl; } };
    t1.join();
    t2.join();
    t3.join();
    prof::profiler::dump_all_threads(std::cout);

    std::ofstream sfile { "output.dmp" };
    prof::profiler::save(sfile);

    std::ifstream rfile { "output.dmp" };
    prof::profiler::load(rfile);

    return 0;
}