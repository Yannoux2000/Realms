#pragma once
#include <chrono>
#include <string>

namespace rlms {
	class LogTime {
	private:
		static std::chrono::steady_clock::time_point _t_start;
	public:
		static std::string GetTime ();
		static std::string GetDateTime ();
	};
}
