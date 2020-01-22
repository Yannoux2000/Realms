#include "DebugConsoleLogger.h"

#include <iostream>
#include "../../_Preprocess.h"
#include "LogTime.h"

using namespace rlms;

DebugConsoleLogger::DebugConsoleLogger () {
	std::cout << "===============================================\n"
		<< "    Begin log ( "
		<< LogTime::GetDateTime ()
		<< " ): "
		<< RLMS_PLATFORM_NAME
		<< "\n===============================================\n";
	std::cout.flush ();
}

DebugConsoleLogger::~DebugConsoleLogger () {
	std::cout << "===============================================\n"
		<< "    Ended Output log ( "
		<< LogTime::GetDateTime ()
		<< " ): "
		<< RLMS_PLATFORM_NAME
		<< "\n===============================================\n\n\n";
	std::cout.flush ();
}

void DebugConsoleLogger::log (const std::string& s, const char& tag, std::string name) {
	std::cout
		<< LogTime::GetTime ()
		<< " : [ " << tag << " ] "
		<< (name.empty() ? "" : "{" + name + "} ")
		<< s <<
	std::endl;
}
