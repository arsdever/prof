#pragma once

#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <frame.hpp>
#include <iostream>
#include <stack>
#include <sstream>

namespace std
{
	inline string to_string(thread::id const& id)
	{
		stringstream ss;
		ss << id;
		return ss.str();
	}
}

namespace prof
{
	class profiler_data_collector;
	class scoped_profiler;

	class profiler
	{
	public:
		using data_t = frame;

		static scoped_profiler profile(std::string_view fname, std::string const& thid = std::to_string(std::this_thread::get_id()));

		static profiler& implementation();
		static void set_implementation(std::unique_ptr<profiler>&& impl);
		void dump(std::ostream& s);

	protected:
		std::shared_ptr<profiler_data_collector> get_collector(std::string_view thid);
		void push_data(data_t const& d);

		void push_frame(std::string_view func);
		void pop_frame();

		data_t::depth_t current_depth() const;

		friend class profiler_data_collector;
		friend class scoped_profiler;

		static std::unique_ptr<profiler> _impl;
		std::unordered_map<std::string, std::shared_ptr<profiler_data_collector>> _collectors;
		std::stack<data_t> _data_stack;
		std::unordered_set<data_t> _data;
		std::mutex _mutex;
	};
}