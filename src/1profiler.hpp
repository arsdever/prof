#pragma once

#include <assert.h>
#include <chrono>
#include <memory>
#include <stack>
#include <string_view>
#include <unordered_map>
#include <thread>

namespace prof {
	class profiler_impl;
	using profiler_impl_uptr = std::unique_ptr<profiler_impl>;
	class profiler_factory;
	using profiler_factory_uptr = std::unique_ptr<profiler_factory>;

	class profiler_impl {
	public:
		virtual void start() = 0;
		virtual void finish() = 0;
		virtual std::chrono::steady_clock::time_point begin() const = 0;
		virtual std::chrono::steady_clock::time_point end() const = 0;
	};

	struct profiler_factory {
		using call_id_t = std::tuple<std::string_view, typename std::thread::id>;
		virtual profiler_impl_uptr operator()() = 0;
	};

	class profiler {
	public:
		profiler(std::string_view name, profiler_impl_uptr&& impl)
			: _impl{ std::move(impl) } {}

		//   void register_function(uint64_t fptr, std::string_view name) {
		//     _name_mapping.insert({fptr, std::string{name}});
		//   }

#define prof_call(p)                                                                 \
  p.fcall(std::make_tuple(std::string_view{FUNCTION_NAME},                       \
                        std::this_thread::get_id()))
#define prof_void_ret(p)                                                                  \
  p.freturn(std::make_tuple(std::string_view{FUNCTION_NAME},                     \
                          std::this_thread::get_id()))
#define prof_ret(p,v)                                                                 \
  p.freturn(std::make_tuple(std::string_view{FUNCTION_NAME},                     \
                          std::this_thread::get_id()),                         \
          v)

#ifdef WIN32
#define FUNCTION_NAME __func__
#else
#define FUNCTION_NAME __PRETTY_FUNCTION__
#endif

		using call_id_t = std::tuple<std::string_view, typename std::thread::id, uint32_t>;
		struct call_id_hash {
			std::size_t operator()(call_id_t const& obj) const {
				return std::hash<std::string_view>{}(std::get<0>(obj)) ^
					std::hash<std::thread::id>{}(std::get<1>(obj)) ^ std::hash<uint32_t>{}(std::get<2>(obj));
			}
		};

		void fcall(call_id_t call_id) {
			auto profiler_stack = _profilers.insert(std::make_pair(call_id, std::stack<profiler_impl_uptr>{}));
			profiler_stack.first->second.push(std::move((*factory)()));
			profiler_for(call_id)->start();
		}

		template <typename T> T&& freturn(call_id_t call_id, T&& obj) {
			profiler_for(call_id)->finish();
			_data.insert({ std::get<0>(call_id),
						  std::chrono::duration<double>(profiler_for(call_id)->end() -
														profiler_for(call_id)->begin())
							  .count() });
			return std::forward<T>(obj);
		}

		void freturn(call_id_t call_id) {
			profiler_for(call_id)->finish();
			_data.insert({ std::get<0>(call_id),
						  std::chrono::duration<double>(profiler_for(call_id)->end() -
														profiler_for(call_id)->begin())
							  .count() });
		}

		profiler_impl_uptr& profiler_for(call_id_t call_id) {
			assert(!_profilers.at(call_id).empty());
			return _profilers.at(call_id).top();
		}

		std::unordered_map<std::string_view, double> const&
			get_statistics_data() const {
			return _data;
		}

		//   std::string const &get_function_name(uint64_t id) const {
		//     return _name_mapping.at(id);
		//   }

		profiler_factory_uptr factory;

	private:
		//   std::unordered_map<uint64_t, std::string> _name_mapping;
		std::unordered_map<call_id_t, std::stack<profiler_impl_uptr>, call_id_hash>
			_profilers;
		std::unordered_map<std::string_view, double> _data;
		profiler_impl_uptr _impl;
	};

	class simple_profiler_impl : public profiler_impl {
	public:
		void start() override { _start = std::chrono::steady_clock::now(); }
		void finish() override { _end = std::chrono::steady_clock::now(); }

		std::chrono::steady_clock::time_point begin() const override {
			return _start;
		}
		std::chrono::steady_clock::time_point end() const override { return _end; }

	private:
		std::chrono::steady_clock::time_point _start;
		std::chrono::steady_clock::time_point _end;
	};

	struct simple_profiler_factory : public profiler_factory {
		profiler_impl_uptr operator()() override {
			return std::make_unique<simple_profiler_impl>();
		}
	};

} // namespace prof
