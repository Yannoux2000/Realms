#pragma once
#include "../../Base/Logging/ILogged.h"

#include "../../CoreTypes.h"

#include <functional>

namespace rlms {
	struct Job {
		uint16_t priority;
		std::function<void ()> job;
		void* args;

		Job () : args (nullptr), priority (std::numeric_limits<uint16_t>::max ()) {}
		Job (std::function<void ()> const& item, void* const& data = nullptr) : job (item), args(data), priority(std::numeric_limits<uint16_t>::max ()) {}

		inline void operator()() {
			if(job) job ();
		}

		inline bool operator<(Job const& other) {
			return priority < other.priority;
		}
	};

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
		void Execute (Job const& job);

		// Add a job to execute asynchronously. Any idle thread will execute this job.
		void Register (Job const& job);

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
