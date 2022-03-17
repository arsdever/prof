#pragma once

#include <string>
#include <list>
#include <chrono>

namespace prof
{
	struct execution_node
	{
		std::string funciton_name;
		std::chrono::steady_clock::time_point start_time;
		std::chrono::steady_clock::time_point end_time;
		bool has_finished;
		std::list<execution_node*> calls;
	};
}