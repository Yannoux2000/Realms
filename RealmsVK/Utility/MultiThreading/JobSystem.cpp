#include "JobSystem.h"
#include "JobSequencer.h"

#include <algorithm>			// std::max
#include <atomic>				// to use std::atomic<uint64_t>
#include <thread>				// to use std::thread
#include <condition_variable>	// to use std::condition_variable

using namespace rlms;

class rlms::JobSystemImpl : public ILogged {
private:
	friend class JobSystem;

	std::string getLogName () {
		return "JobSystem";
	}

	uint32_t numThreads = 0;							// number of worker threads, it will be initialized in the Initialize() function
	JobSequencer<256> jobSequencer;						// a thread safe queue to put pending jobs onto the end. A worker thread can grab a job from the beginning
	std::condition_variable wakeCondition;				// used in conjunction with the wakeMutex below. Worker threads just sleep when there is no job, and the main thread can wake them up
	std::mutex wakeMutex;								// used in conjunction with the wakeCondition above
	uint64_t currentLabel = 0;							// tracks the state of execution of the main thread
	std::atomic_uint16_t finishedLabel;					// track the state of execution across background worker threads


	void start (std::shared_ptr<Logger> funnel);
	void stop ();

	void poll ();

	// Add a job to execute asynchronously. Any idle thread will execute this job.
	void execute (Job const& job);

	void register_job (Job const& job);

	// Divide a job onto multiple jobs and execute in parallel.
	//	jobCount	: how many jobs to generate for this task.
	//	groupSize	: how many jobs to execute per thread. Jobs inside a group execute serially. It might be worth to increase for small jobs
	//	func		: receives a JobDispatchArgs as parameter
	void dispatch (uint32_t jobCount, uint32_t groupSize, const std::function<void (JobDispatchArgs)>& job);

	// Check if any threads are working currently or not
	bool isBusy ();


public:
	JobSystemImpl ();
	~JobSystemImpl ();
};

std::unique_ptr<rlms::JobSystemImpl> JobSystem::instance;

std::shared_ptr<LoggerHandler> rlms::JobSystem::GetLogger () {
	return instance->getLogger ();
}

void rlms::JobSystem::Initialize (std::shared_ptr<Logger> funnel) {
	instance = std::make_unique<JobSystemImpl> ();
	instance->start (funnel);
}

void rlms::JobSystem::Execute (Job const& job) {
	instance->execute (job);
}

void rlms::JobSystem::Register (Job const& job) {
	instance->register_job(job);
}

void rlms::JobSystem::Dispatch (uint32_t jobCount, uint32_t groupSize, const std::function<void (JobDispatchArgs)>& job) {
	instance->dispatch (jobCount, groupSize, job);
}

bool rlms::JobSystem::IsBusy () {
	return instance->isBusy();
}

void rlms::JobSystem::Terminate () {
	instance->stop ();
	instance.reset ();
}

void rlms::JobSystemImpl::start (std::shared_ptr<Logger> funnel) {
	startLogger (funnel);
	logger->tag (LogTags::None) << "Initializing !" << '\n';

	// Initialize the worker execution state to 0:
	finishedLabel.store (0);

	auto numCores = std::thread::hardware_concurrency ();
	numThreads = std::max (1u, numCores);
	logger->tag (LogTags::Info) << numThreads << " Threads counted for " << numCores << "Processing units.\n";

	//starting workers
	for (uint32_t threadID = 0; threadID < numThreads; ++threadID) {
		std::thread worker ([this] {
			Job* job = nullptr; // the current job for the thread, it's empty at start.

			while (true) {
				if (jobSequencer.elect_job (job)) // try to grab a job from the jobSequencer queue
				{
					// It found a job, execute it:
					job->operator()();
					finishedLabel.fetch_add (1); // update worker label state
				} else {
					// no job, put thread to sleep
					std::unique_lock<std::mutex> lock (wakeMutex);
					wakeCondition.wait (lock);
				}
			}
			});
		// *****Here we could do platform specific thread setup...
		worker.detach (); // forget about this thread, let it do it's job in the infinite loop that we created above
	}

	logger->tag (LogTags::None) << "Initialized correctly !" << '\n';
}

void rlms::JobSystemImpl::stop () {
	logger->tag (LogTags::None) << "Stopping" << '\n';

	while (isBusy ()) { poll (); }

	logger->tag (LogTags::None) << "Stopped correctly !" << '\n';
}

inline void rlms::JobSystemImpl::poll ()
{
	wakeCondition.notify_one (); // wake one worker thread
	std::this_thread::yield (); // allow this thread to be rescheduled
}

void rlms::JobSystemImpl::execute (Job const& job) {
	// The main thread label state is updated:
	currentLabel += 1;

	// Try to push a new job until it is pushed successfully:
	while (!jobSequencer.add_job (job)) { poll (); }

	wakeCondition.notify_one (); // wake one thread
}

void rlms::JobSystemImpl::register_job (Job const& job) {
	// The main thread label state is updated:
	currentLabel += 1;

	// Try to push a new job until it is pushed successfully:
	while (!jobSequencer.add_job (job)) { poll (); }
}

void rlms::JobSystemImpl::dispatch (uint32_t jobCount, uint32_t groupSize, const std::function<void (JobDispatchArgs)>& job) {
	//if (jobCount == 0 || groupSize == 0)
	//{
	//	return;
	//}

	//// Calculate the amount of job groups to dispatch (overestimate, or "ceil"):
	//const uint32_t groupCount = (jobCount + groupSize - 1) / groupSize;

	//// The main thread label state is updated:
	//currentLabel += groupCount;

	//for (uint32_t groupIndex = 0; groupIndex < groupCount; ++groupIndex) {
	//	// For each group, generate one real job:
	//	auto& jobGroup = [jobCount, groupSize, job, groupIndex]() {

	//		// Calculate the current group's offset into the jobs:
	//		const uint32_t groupJobOffset = groupIndex * groupSize;
	//		const uint32_t groupJobEnd = std::min (groupJobOffset + groupSize, jobCount);

	//		JobDispatchArgs args;
	//		args.groupIndex = groupIndex;

	//		// Inside the group, loop through all job indices and execute job for each index:
	//		for (uint32_t i = groupJobOffset; i < groupJobEnd; ++i)
	//		{
	//			args.jobIndex = i;
	//			job (args);
	//		}
	//	};

	//	Job j(jobGroup);

	//	// Try to push a new job until it is pushed successfully:
	//	while (!jobSequencer.add_job (j)) { poll (); }

	//	wakeCondition.notify_one (); // wake one thread
	//}
}

bool rlms::JobSystemImpl::isBusy () {
	// Whenever the main thread label is not reached by the workers, it indicates that some worker is still alive
	return finishedLabel.load () < currentLabel;
}

rlms::JobSystemImpl::JobSystemImpl () {}
rlms::JobSystemImpl::~JobSystemImpl () {}
