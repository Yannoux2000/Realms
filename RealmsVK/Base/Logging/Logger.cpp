#include "Logger.h"
#include "../../_Preprocess.h"

#include <sstream>

using namespace rlms;

bool LogTags::_noneDisp = true;
bool LogTags::_infoDisp = true;
bool LogTags::_debugDisp = true;
bool LogTags::_devDisp = true;
bool LogTags::_errorDisp = true;
bool LogTags::_warnDisp = true;

void LogTags::setTagsDisplay (bool t_none_disp, bool t_info_disp, bool t_debug_disp, bool t_dev_disp, bool t_error_disp, bool t_warn_disp) {
	_noneDisp = t_none_disp || t_info_disp;
	_infoDisp = t_info_disp;
	_debugDisp = t_debug_disp;
	_devDisp = t_dev_disp || RLMS_DEBUG_BOOL;
	_errorDisp = t_error_disp || t_warn_disp || t_info_disp;
	_warnDisp = t_warn_disp;
}

bool LogTags::Displayable (const char current_tag) {
	return ((current_tag == None && _noneDisp) ||
			(current_tag == Info && _infoDisp) ||
			(current_tag == Debug && _debugDisp) ||
			(current_tag == Dev && _devDisp) ||
			(current_tag == Error && _errorDisp) ||
			(current_tag == Warn && _warnDisp)
		);
}

void Logger::check_line_ended () {
	size_t found = _current_line.find ('\n');
	if (found != std::string::npos) {
		log (_current_line.substr (0, found), _tag, _disp_name);
		_current_line = _current_line.substr (found + 1);
		_tag = LogTags::None;
		_disp_name = "";
		check_line_ended ();
	}
}

Logger& Logger::endl () {
	if (LogTags::Displayable(_tag)) {
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
