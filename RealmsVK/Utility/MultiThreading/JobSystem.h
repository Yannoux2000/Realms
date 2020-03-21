#pragma once
#include "../../Base/Logging/ILogged.h"
#include "Job.h"

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
		template<class JobType> static void Register (JobType&& job);
		template<class JobType> static void Execute (JobType&& job);

		static void Register (IJob* j);
		static void Execute (IJob* j);

		static void Reset ();
		static void WakeUp (uint8_t n = 0);

		static void MainWorker ();
		static void CaptureMainWorker ();
		static void FreeMainThread ();

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

	template<class JobType> void JobSystem::Register (JobType&& job) {
		JobType* j = new JobType (std::move(job));
		Register (static_cast<IJob*>(j));
	}

	template<class JobType> void JobSystem::Execute (JobType&& job) {
		JobType* j = new JobType (std::move(job));
		Execute (static_cast<IJob*>(j));
	}
}
