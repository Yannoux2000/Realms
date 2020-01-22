#pragma once
#include <string>
#include <memory>

//#include "../../Base/Logging/LoggerHandler.h"
//#include "../../Base/RlmsException.h"

namespace rlms {

	class ThreadPoolImpl;

	class ThreadPool {
	private:
		static std::unique_ptr<ThreadPoolImpl> instance;

		static std::shared_ptr<LoggerHandler> GetLogger ();
		static bool Initialize (std::shared_ptr<Logger> funnel = nullptr);
		static void Terminate ();

	};
}

