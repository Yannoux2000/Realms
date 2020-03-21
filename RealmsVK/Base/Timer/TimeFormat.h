#pragma once

#include "../../CoreTypes.h"
#include <string>

namespace rlms {
	class TimeFormat {
	public:
		static std::string Precise (DURATION d);
		static std::string Human (DURATION d);
		static std::string NowDate ();
	};
}
