#pragma once
#include "../../Base/Logging/ILogged.h"

#include "../../CoreTypes.h"

#include <functional>

namespace rlms {
	struct Job {
		JOB_PRIORITY_TYPE priority;
		std::function<void (void*)> job;
		void* args;

		Job () : args (nullptr), priority (JOB_MIN_PRIORITY) {}

		Job (std::function<void ()> const& item, JOB_PRIORITY_TYPE const n = JOB_MIN_PRIORITY, void* const& data = nullptr) : args (data), priority (n) {
			job = [item](void* _discated) {item (); };
		}

		Job (std::function<void (void*)> const& item, JOB_PRIORITY_TYPE const n = JOB_MIN_PRIORITY, void* const& data = nullptr) : job (item), args (data), priority (n) {}

		inline void operator()() {
			if (job != nullptr) {
				job (args);
			} else throw std::exception ("invalid job");
		}

		inline bool operator<(Job const& other) {
			return priority < other.priority;
		}

		inline bool operator>(Job const& other) {
			return priority > other.priority;
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
		static void Execute (Job const job);

		static void Reset ();
		static void Pass (uint8_t n = 0);

		// Add a job to execute asynchronously. Any idle thread will execute this job.
		static void Register (Job const job);

		// Divide a job onto multiple jobs and execute in parallel.
		//	jobCount	: how many jobs to generate for this task.
		//	groupSize	: how many jobs to execute per thread. Jobs inside a group execute serially. It might be worth to increase for small jobs
		//	func		: receives a JobDispatchArgs as parameter
		static void Dispatch (uint32_t jobCount, uint32_t groupSize, const std::function<void (JobDispatchArgs)>& job);

		// Check if any threads are working currently or not
		static bool IsBusy ();

		static void Terminate ();
	private:
		static std::unique_ptr<JobSystemImpl> instance;
	};
}
