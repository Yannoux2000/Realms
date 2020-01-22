#pragma once

#include "Logger.h"
#include <fstream>

namespace rlms {
	class FileLogger : public Logger {
	private:
		std::ofstream _file;

	public:
		FileLogger (std::string filename);
		~FileLogger ();

		void log (const std::string& s, const char& tag, std::string name = "");
	};
}