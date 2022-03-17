#include "profiler.hpp"
#include <iostream>
#include <thread>

class unit {
public:
	unit()
		: _profiler{ "unit",
					std::move(std::make_unique<prof::simple_profiler_impl>()) } {
		_profiler.factory = std::move(std::make_unique<prof::simple_profiler_factory>());
		// _profiler.register_function(0, "long_running_task");
		// _profiler.register_function(1, "slow_motion");
	}

	void slow_motion() {
		prof_call(_profiler);
		static int a[]{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
		while (std::ranges::next_permutation(a).found) {
		} // generates 12! permutations
		prof_void_ret(_profiler);
	}

	std::string long_running_task(int n = 10) {
		prof_call(_profiler);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		if (n)
			long_running_task(n - 1);

		return prof_ret(_profiler, "Hello");
	}

public:
	prof::profiler _profiler;
};

int main(int argc, char** argv) {
	unit u{};
	std::cout << u.long_running_task() << std::endl;
	u.slow_motion();
	for (auto stat_data : u._profiler.get_statistics_data())
	{
		std::cout << "The execution of " << stat_data.first << " took " << stat_data.second
			<< "s" << std::endl;

	}
	return 0;
}