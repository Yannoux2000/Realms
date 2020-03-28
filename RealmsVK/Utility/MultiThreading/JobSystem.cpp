#include "JobSystem.h"
#include "PriorityFIFOJobScheduler.h"

#include <algorithm>			// std::max
#include <atomic>				// to use std::atomic<uint64_t>
#include <thread>				// to use std::thread
#include <condition_variable>	// to use std::condition_variable

#include <vector>

using namespace rlms;

class rlms::JobSystemImpl : public ILogged {
private:
	friend class JobSystem;

	std::string getLogName () {
		return "JobSystem";
	}

	std::vector<std::thread*> threads;

	PriorityFIFOJobScheduler jobSched;							// a thread safe queue to put pending jobs onto the end. A worker thread can grab a job from the beginning
	std::condition_variable wakeCondition;				// used in conjunction with the wakeMutex below. Worker threads just sleep when there is no job, and the main thread can wake them up
	std::mutex wakeMutex;								// used in conjunction with the wakeCondition above

	std::atomic_bool endSignal;							// signal to stop all threads
	std::atomic_bool endMainSignal;						// signal to stop main thread

	uint64_t currentLabel = 0;							// tracks the state of execution of the main thread
	std::atomic_uint16_t finishedLabel;					// track the state of execution across background worker threads

	uint32_t numThreads;								// number of worker threads, it will be initialized in the Initialize() function
	std::atomic_uint16_t idleWorkers;					// track the state of execution across background worker threads

	void start (std::shared_ptr<Logger> funnel);
	void stop ();
	void poll ();

	// Add a job to execute asynchronously. Any idle thread will execute this job.
	void execute_job (IJob* job);
	void reset ();
	void pass (uint8_t n);
	void register_job (IJob* job);

	void main_worker ();

	void main_worker_loop ();
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

void rlms::JobSystem::Register (IJob* j) {
	instance->register_job (j);
}

void rlms::JobSystem::Execute (IJob* j) {
	instance->execute_job (j);
}

void rlms::JobSystem::Reset () {
	instance->reset ();
}

void rlms::JobSystem::WakeUp (uint8_t n) {
	instance->pass (n);
}

void rlms::JobSystem::MainWorker () {
	instance->main_worker ();
}

void rlms::JobSystem::CaptureMainWorker () {
	instance->main_worker_loop ();
}

void rlms::JobSystem::FreeMainThread () {
	instance->endMainSignal.store (true);
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
	endSignal.store (false);

	auto numCores = std::thread::hardware_concurrency ();
	numThreads = std::max (1u, numCores - 1);
	logger->tag (LogTags::Info) << numThreads << " Threads counted for " << numCores << " Processing units.\n";

	//starting workers
	for (uint32_t threadID = 0; threadID < numThreads; ++threadID) {
		logger->tag (LogTags::Info) <<"Creating thread with id : " << threadID << "\n";
		std::thread* worker = new std::thread ([this] {
			IJob* job; // the current job for the thread, it's empty at start.

			while (!endSignal) {
				if (jobSched.elect_job (job)) // try to grab a job from the jobSequencer queue
				{
					// It found a job, execute it:
					job->operator()();
					delete job;

					finishedLabel.fetch_add (1); // update worker label state
				} else {
					idleWorkers.fetch_add (1);
					// no job, put thread to sleep
					std::unique_lock<std::mutex> lock (wakeMutex);
					wakeCondition.wait_for (lock, std::chrono::milliseconds(10));

					idleWorkers.fetch_sub (1);
				}
			}
			});
		// *****Here we could do platform specific thread setup...

		threads.push_back (worker);
		//worker->detach (); // forget about this thread, let it do it's job in the infinite loop that we created above
	}

	logger->tag (LogTags::None) << "Initialized correctly !" << '\n';
}

void rlms::JobSystemImpl::stop () {
	//wait for all jobs to end
	while (isBusy ()) {
		poll ();
	}
	logger->tag (LogTags::None) << "Stopping" << '\n';

	//send end signal
	endSignal.store (true);
	wakeCondition.notify_all ();

	for (auto it = threads.begin(); it != threads.end(); it++) {
		(*it)->join ();
		delete *it;
	}

	logger->tag (LogTags::None) << "Stopped correctly !" << '\n';
}

inline void rlms::JobSystemImpl::poll () {
	wakeCondition.notify_one (); // wake one worker thread
	std::this_thread::yield (); // allow this thread to be rescheduled
}

void rlms::JobSystemImpl::execute_job (IJob* job) {
	// The main thread label state is updated:
	currentLabel += 1;

	// Try to push a new job until it is pushed successfully:

	while (!jobSched.add_job (job)) {
		poll ();
	}

	wakeCondition.notify_one (); // wake one thread
}

void rlms::JobSystemImpl::reset () {
	jobSched.reset ();
}

void rlms::JobSystemImpl::pass (uint8_t n) {

	logger->tag (LogTags::None) << "Waking ";
	
	if (n == 0) {
		*logger << "all threads.\n";
		wakeCondition.notify_all ();
	} else {
		*logger << n << " threads.\n";
		for (uint8_t i = 0; i < n; i++) {
			wakeCondition.notify_one ();
		}
	}
}

void rlms::JobSystemImpl::register_job (IJob* job) {
	// The main thread label state is updated:
	currentLabel += 1;

	// Try to push a new job until it is pushed successfully:
	while (!jobSched.add_job (job)) {
		poll ();
	}
}

void rlms::JobSystemImpl::main_worker () {
	IJob* job = nullptr; // the current job for the thread, it's empty at start.
	if (jobSched.elect_job (job)) // try to grab a job from the jobSequencer queue
	{
		// It found a job, execute it:
		job->operator()();
		delete job;
		finishedLabel.fetch_add (1); // update worker label state
	}
}

void rlms::JobSystemImpl::main_worker_loop () {
	IJob* job = nullptr; // the current job for the thread, it's empty at start.

	while (!endSignal && !endMainSignal) {
		if (jobSched.elect_job (job)) // try to grab a job from the jobSequencer queue
		{
			// It found a job, execute it:
			job->operator()();
			delete job;

			finishedLabel.fetch_add (1); // update worker label state
		} else {
			idleWorkers.fetch_add (1);
			// no job, put thread to sleep
			std::unique_lock<std::mutex> lock (wakeMutex);
			wakeCondition.wait (lock);

			idleWorkers.fetch_sub (1);
		}
	}
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
