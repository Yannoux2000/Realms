#include "LogTime.h"
#include <iomanip>
#include <sstream>

using namespace rlms;

std::chrono::steady_clock::time_point LogTime::_t_start = std::chrono::steady_clock::now ();

std::string LogTime::GetTime () {
	std::ostringstream oss;

	auto duration = std::chrono::steady_clock::now () - _t_start;

	auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
	duration -= hours;
	auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
	duration -= minutes;
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
	duration -= seconds;
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);

	oss << std::setfill ('0') << std::setw (2) << hours.count () << ':';
	oss << std::setfill ('0') << std::setw (2) << minutes.count () << ':';
	oss << std::setfill ('0') << std::setw (2) << seconds.count () << "::";
	oss << std::setfill ('0') << std::setw (3) << milliseconds.count ();

	return oss.str ();
}

std::string LogTime::GetDateTime () {
	std::ostringstream oss;
	auto now = std::chrono::system_clock::to_time_t (std::chrono::system_clock::now ());

	std::tm bt{ };
#if defined(__unix__)
	localtime_r (&timer, &bt);
#elif defined(_MSC_VER)
	localtime_s (&bt, &now);
#else
	static std::mutex mtx;
	std::lock_guard<std::mutex> lock (mtx);
	bt = *std::localtime (&timer);
#endif
	oss << std::put_time (&bt, "%c");
	return oss.str ();
}