#pragma once

#include "LoggerHandler.h"
#include <memory>
#include <string>

namespace rlms {
	class ILogged {
	protected:
		std::shared_ptr<LoggerHandler> logger;
		virtual std::string getLogName () {
			return typeid(*this).name ();
		};
		const std::shared_ptr<LoggerHandler> getLogger ();
		void startLogger (std::shared_ptr<Logger> funnel = nullptr, bool display = false);
		void stopLogger ();
	};
}