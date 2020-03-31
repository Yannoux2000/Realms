#pragma once

#include "Logger.h"

#include <list>
#include <memory>

namespace rlms {
	////////////////////////////////////////////////////////////
	/// \brief Logger meant to be used to multiplex
	///        streams to multiple logger.
	///	       channelIn output loggers to an instance to
	///        connect.
	///        
	////////////////////////////////////////////////////////////

	class LoggerHandler : public std::list<std::shared_ptr<Logger>>, public Logger {
	private:
		std::mutex _lock;
		std::string _name;
	public:
		LoggerHandler& tag (const char& tag);
		const std::string disp_name () const;

		const std::string disp_name (std::string disp_name) const;

		void name (std::string name) {
			_name = name;
		}
		std::string& name () {
			return _name;
		}

		LoggerHandler ();
		LoggerHandler (std::string name);
		~LoggerHandler ();

		void log (const std::string& s, const char& tag, std::string disp_name = "") override;

		void print (const char& tag = LogTags::None, std::string const& s = "") {
			_lock.lock ();
			log (s, tag, disp_name ());
			_lock.unlock ();
		}

		void channelIn (std::shared_ptr<Logger>);
		template<class L, class ... Args> void channelIn (Args&& ... args);
		template<typename T> LoggerHandler& operator<< (T data);
	};

	template<class L, class ...Args>
	inline void LoggerHandler::channelIn (Args&& ...args) {
		auto logger = std::shared_ptr<Logger> (new L (std::forward<Args> (args)...));
		push_back (logger);
		back ()->tag (Logger::tag ());
	}

	template<typename T>
	inline LoggerHandler& LoggerHandler::operator<<(T data) {
		for (auto it = begin (); it != end (); it++) {
			(*it)->operator<<(data);
		}
		return (*this);
	}
}