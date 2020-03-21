#include "TimeFormat.h"
#include <chrono>
#include <iomanip>
#include <sstream>

std::string rlms::TimeFormat::Precise (DURATION d) {
	std::ostringstream oss;

	auto hours = std::chrono::duration_cast<std::chrono::hours>(d);
	d -= hours;
	auto minutes = std::chrono::duration_cast<std::chrono::minutes>(d);
	d -= minutes;
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(d);
	d -= seconds;
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(d);

	oss << std::setfill ('0') << std::setw (2) << hours.count () << ':';
	oss << std::setfill ('0') << std::setw (2) << minutes.count () << ':';
	oss << std::setfill ('0') << std::setw (2) << seconds.count () << "::";
	oss << std::setfill ('0') << std::setw (3) << milliseconds.count ();

	return oss.str ();
}

std::string rlms::TimeFormat::Human (DURATION d) {
	std::ostringstream oss;

	auto hours = std::chrono::duration_cast<std::chrono::hours>(d);
	d -= hours;
	auto minutes = std::chrono::duration_cast<std::chrono::minutes>(d);
	d -= minutes;
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(d);

	oss << std::setfill ('0') << std::setw (2) << hours.count () << ':';
	oss << std::setfill ('0') << std::setw (2) << minutes.count () << ':';
	oss << std::setfill ('0') << std::setw (2) << seconds.count ();

	return oss.str ();
}

std::string rlms::TimeFormat::NowDate () {
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
