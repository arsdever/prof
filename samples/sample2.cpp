#include <iostream>
#include <thread>
#include "execution_tree.hpp"

profiler::execution_node* current;

#define START_PROFILE \
profiler::execution_node* old = std::exchange(current, new profiler::execution_node());\
old->calls.push_back(current);														   \
current->funciton_name = __func__;													   \
current->start_time = std::chrono::steady_clock::now();								   \

#define FINISH_PROFILE \
current->end_time = std::chrono::steady_clock::now();								   \
current = old;

class unit {
public:
	unit() {}

	void slow_motion() {
		START_PROFILE;

		static int a[]{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
		while (std::ranges::next_permutation(a).found) {
		} // generates 12! permutations

		FINISH_PROFILE;
	}

	std::string long_running_task(int n = 10) {
		START_PROFILE;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		if (n)
			long_running_task(n - 1);

		FINISH_PROFILE;
		return "hello";
	}
};

void print_node(profiler::execution_node const* node, uint32_t indent = 0)
{
	std::cout << std::string(indent, ' ') << node->funciton_name << ": " << std::chrono::duration<double>(node->end_time - node->start_time) << std::endl;
	for (profiler::execution_node const* call : node->calls)
	{
		print_node(call, indent + 1);
	}
}

int main(int argc, char** argv) {
	unit u{};
	profiler::execution_node n;
	current = &n;
	n.funciton_name = "main";
	std::cout << u.long_running_task() << std::endl;
	u.slow_motion();

	print_node(current);

	return 0;
}