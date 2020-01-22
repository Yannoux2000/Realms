#include "Logger.h"
#include "../../_Preprocess.h"

#include <sstream>

using namespace rlms;

void Logger::check_line_ended () {
	size_t found = _current_line.find ('\n');
	if (found != std::string::npos) {
		log (_current_line.substr (0, found), _tag, _disp_name);
		_current_line = _current_line.substr (found+1);
		_tag = LogTags::None;
		_disp_name = "";
		check_line_ended ();
	}
}

Logger& Logger::endl () {
	if(_tag != LogTags::Dev || RLMS_DEBUG_BOOL) {
		log (_current_line, _tag, _disp_name);
	}
	reset ();
	return *this;
}

void Logger::reset () {
	_current_line = "";
	_tag = LogTags::None;
	_disp_name = "";
}