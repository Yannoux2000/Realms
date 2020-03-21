#pragma once
#include <chrono>
#include "../../CoreTypes.h"

namespace rlms {
	class ITimer {
	public:
		//using steady_clock because it will be used to mesure application time
		using chrono = std::chrono::steady_clock;
		using time_point = std::chrono::time_point<chrono>;

		void start (time_point s) {
			_start = s;
		}
		const time_point start () {
			return _start;
		}
		//time_point now ();
		//virtual DURATION since (time_point const t = _start) {

		//}

	private:
		static time_point _start;
	};
}
