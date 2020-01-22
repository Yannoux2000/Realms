#include "ILogged.h"
#include "FileLogger.h"
#include "DebugConsoleLogger.h"

using namespace rlms;

const std::shared_ptr<LoggerHandler> ILogged::getLogger () {
	return logger;
}

void ILogged::startLogger (std::shared_ptr<Logger> funnel, bool display) {
	auto name = getLogName ();
	logger = std::make_shared<LoggerHandler> (name);

	if (funnel != nullptr) {
		logger->channelIn (funnel);
	}

	logger->channelIn<FileLogger> ("Logs/" + name);
	if (display) {
		logger->channelIn<DebugConsoleLogger> ();
	}
}

void ILogged::stopLogger () {
	logger.reset ();
}
