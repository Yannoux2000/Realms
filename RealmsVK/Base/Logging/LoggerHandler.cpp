#include "LoggerHandler.h"

using namespace rlms;

LoggerHandler& LoggerHandler::tag (const char& tag) {
	Logger::tag (tag);
	for (auto it = begin (); it != end (); it++) {
		(*it)->tag (tag);
		(*it)->disp_name (LoggerHandler::disp_name ());
	}
	return *this;
}

const std::string LoggerHandler::disp_name () const {
	return _name + (Logger::disp_name ().empty () ? "" : '.' + Logger::disp_name ());
}

const std::string LoggerHandler::disp_name (std::string disp_name) const {
	return _name + (Logger::disp_name ().empty () ? "" : '.' + Logger::disp_name ());
}

LoggerHandler::LoggerHandler () : _name (""), Logger(true) {}

LoggerHandler::LoggerHandler (std::string name) : _name (name), Logger (true) {}

LoggerHandler::~LoggerHandler () {
	for (auto it = begin (); it != end (); it++) {
		it->reset ();
	}
}

void LoggerHandler::log (const std::string& s, const char& tag, std::string disp_name) {
	_lock.lock ();
	for (auto it = begin (); it != end (); it++) {
		(*it)->log (s, tag, LoggerHandler::disp_name ());
	}
	_lock.unlock ();
}

void LoggerHandler::channelIn (std::shared_ptr<Logger> logger) {
	push_back(logger);
}
