#include "ApplicationTime.h"
#include <sstream>

using namespace rlms;

ApplicationTime::time_point ApplicationTime::_start;

void ApplicationTime::Initialize () {
	_start = chrono::now ();
}

const ApplicationTime::time_point ApplicationTime::Start () {
	return _start;
}

ApplicationTime::time_point ApplicationTime::Now () {
	return chrono::now ();
}

DURATION rlms::ApplicationTime::Since (time_point const t_start) {
	return std::chrono::duration_cast<DURATION>(chrono::now () - t_start);
}
