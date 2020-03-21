#pragma once

#include <chrono>
#include "../../CoreTypes.h"

namespace rlms {
	class ApplicationTime {
	public:

		//using steady_clock because it will be used to mesure application time
		using chrono = std::chrono::steady_clock;
		using time_point = std::chrono::time_point<chrono>;

		static void Initialize ();
		static const time_point Start ();
		static time_point Now ();
		static DURATION Since (time_point const t_start = _start);

	private:
		static time_point _start;
	};
}

