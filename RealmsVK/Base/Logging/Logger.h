#pragma once
#include <string>
#include <sstream>

namespace rlms {
	class LogTags {
	private:
		//flags for displaying or not info types
		static bool _noneDisp;
		static bool _infoDisp;
		static bool _debugDisp;
		static bool _devDisp;
		static bool _errorDisp;
		static bool _warnDisp;

	public:
		static constexpr char None = ' ';
		static constexpr char Info = '?';
		static constexpr char Debug = '=';
		static constexpr char Dev = 'O';
		static constexpr char Error = 'X';
		static constexpr char Warn = '!';

		static void setTagsDisplay (
			bool t_none_disp = true,
			bool t_info_disp = true,
			bool t_debug_disp = true,
			bool t_dev_disp = true,
			bool t_error_disp = true,
			bool t_warn_disp = true);
	};

	class Logger {
	private:
		std::string _current_line;
		std::string _disp_name;
		char _tag;

		void check_line_ended ();
		Logger& endl ();
	protected:
		bool _is_handler;
	public:
		Logger () :_tag (LogTags::None), _disp_name (""), _current_line (""), _is_handler (false) {};
		Logger (bool handler) :_tag (LogTags::None), _disp_name (""), _current_line (""), _is_handler (true) {};

		virtual ~Logger () {};
		virtual void log (const std::string& s, const char& tag = LogTags::None, std::string name = "") = 0;

		Logger& tag (const char& tag) {
			_tag = tag;
			return *this;
		}

		const char& tag () const {
			return _tag;
		}

		Logger& disp_name (const std::string disp_name) {
			_disp_name = disp_name;
			return *this;
		}

		const std::string disp_name () const {
			return _disp_name;
		}

		void reset ();
		template<typename T> Logger& operator<<(T data) {
			std::ostringstream oss;
			oss << _current_line << data;
			_current_line = oss.str ();
			check_line_ended ();
			return *this;
		}
	};
}