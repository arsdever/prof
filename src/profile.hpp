#pragma once

namespace profile
{
	template <typename T>
	class profilable
	{
	public:
		profilable();

		void function_started(std::string_view function_name) {  }
		void function_ended(std::string_view function_name) {
			profiler_manager::report(*this)
		}

	private:
		std::unordered_map<std::string, profiler_data_storage<T::data_t>> _data;
	};

	template <typename T>
	struct profiler;
}