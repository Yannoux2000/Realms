#pragma once
#include <string>
#include <sstream>

namespace rlms {
	namespace LogTags {
		constexpr char None = ' ';
		constexpr char Info = '?';
		constexpr char Debug = '=';
		constexpr char Dev = 'O';
		constexpr char Error = 'X';
		constexpr char Warning = '!';
	}

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