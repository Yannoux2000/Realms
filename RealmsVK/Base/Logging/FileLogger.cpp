#include "FileLogger.h"

#include "../../_Preprocess.h"
#include "../Timer/TimeFormat.h"
#include "../Timer/ApplicationTime.h"

using namespace rlms;
FileLogger::FileLogger (std::string filename) {
	_file.open (filename + ".log", std::ios::out);
	_file.seekp (std::ios::beg);

	if (!_file.good ()) return;
	_file << "===============================================\n"
		<< "    Begin log ( "
		<< TimeFormat::NowDate ()
		<< " ): "
		<< RLMS_PLATFORM_NAME
		<< "\n===============================================\n";
	_file.flush ();
}

FileLogger::~FileLogger () {
	if (!_file.good ()) return;
	_file << "===============================================\n"
		<< "    Ended Output log ( "
		<< TimeFormat::NowDate()
		<< " ): "
		<< RLMS_PLATFORM_NAME
		<< "\n===============================================\n\n\n";
	_file.flush ();
	_file.close ();
}

void FileLogger::log (const std::string& s, const char& tag, std::string name) {
	_file
		<< TimeFormat::Precise (ApplicationTime::Since ())
		<< " : [ " << tag << " ] " 
		<< (name.empty () ? "" : "{" + name + "} ")
		<< s << std::endl;
	_file.flush ();
};
