#pragma once
#include "../../Base/Logging/ILogged.h"

#include "../../CoreTypes.h"

#include <functional>

namespace rlms {

	struct JobDispatchArgs {
		uint32_t jobIndex;
		uint32_t groupIndex;
	};

	class JobSystemImpl;

	class JobSystem {
	public:

		static std::shared_ptr<LoggerHandler> GetLogger ();
		static int n_error;

		static void Initialize (std::shared_ptr<Logger> funnel = nullptr);

		// Add a job to execute asynchronously. Any idle thread will execute this job.
		void Execute (const std::function<void ()>& job);

		// Divide a job onto multiple jobs and execute in parallel.
		//	jobCount	: how many jobs to generate for this task.
		//	groupSize	: how many jobs to execute per thread. Jobs inside a group execute serially. It might be worth to increase for small jobs
		//	func		: receives a JobDispatchArgs as parameter
		void Dispatch (uint32_t jobCount, uint32_t groupSize, const std::function<void (JobDispatchArgs)>& job);

		// Check if any threads are working currently or not
		bool IsBusy ();

		static void Terminate ();
	private:
		static std::unique_ptr<JobSystemImpl> instance;
	};
}
