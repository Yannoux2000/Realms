#pragma once

#include "Logger.h"
#include <string>

namespace rlms {
	class DebugConsoleLogger : public Logger {
	public:
		DebugConsoleLogger ();
		~DebugConsoleLogger ();
		void log (const std::string& s, const char& tag, std::string name = "");
	};
}