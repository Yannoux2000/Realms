#pragma once
#include "Job.h"
#include "JobSystem.h"
#include "../../Base/Timer/ApplicationTime.h"
#include "../../CoreTypes.h"
#include <atomic>

namespace rlms {
	struct DelayedJob : public IJob {
		ApplicationTime::time_point t_start;

		DelayedJob () : IJob (), t_start (ApplicationTime::Now ()) {}
		DelayedJob (std::function<void ()> item) : IJob (item), t_start (ApplicationTime::Now ()) {}
		DelayedJob (std::function<void ()>&& item, ApplicationTime::time_point t_t) : IJob (item), t_start (t_t) {}
		DelayedJob (DelayedJob && j) noexcept : IJob (std::move(j)), t_start (std::move(j.t_start)) {}

		void operator()() override {
			if (job) {
				job ();
			} else throw std::exception ("invalid job");
		}

		bool available () override {
			return ApplicationTime::Now () < t_start;
		}

		JOB_PRIORITY_TYPE const getPriority () const override {
			return (ApplicationTime::Now () < t_start ? JOB_MAX_PRIORITY : JOB_MIN_PRIORITY);
			//return static_cast<JOB_PRIORITY_TYPE>(std::max((d - ApplicationTime::Since (t_prev)).count (), JOB_MIN_PRIORITY));
		}
	};

	template<class period = std::ratio<1>> struct PeriodicJob : public IJob {
		const std::chrono::duration<double, period> d = std::chrono::duration<double, period>(1);
		ApplicationTime::time_point t_start;
		ApplicationTime::time_point t_end;

		std::atomic_bool& kill_signal;

		PeriodicJob (std::function<void ()> item, std::atomic_bool& signal) 
			: IJob (item), t_start (ApplicationTime::Now ()), t_end (ApplicationTime::Now ()), kill_signal(signal) {
			t_end += std::chrono::duration_cast<ApplicationTime::chrono::duration>(d);
		}
		PeriodicJob (std::function<void ()>&& item, ApplicationTime::time_point t_start, std::atomic_bool& signal) 
			: IJob (item), t_start (t_start), t_end (t_start), kill_signal(signal) {
			t_end += std::chrono::duration_cast<ApplicationTime::chrono::duration>(d);
		}
		PeriodicJob (PeriodicJob && j) noexcept 
			: IJob(std::move(j)), t_start(std::move(j.t_start)), t_end (std::move(j.t_end)), kill_signal(j.kill_signal) {}

		void operator()() override {
			if (job != nullptr) {
				job ();
			} else throw std::exception ("invalid job");

			if(kill_signal != true) {
				JobSystem::Register (PeriodicJob<period> (std::move(job), std::max(t_end, ApplicationTime::Now()), kill_signal));
			}
		}

		bool available () override {
			return ApplicationTime::Now () > t_start;
		}

		JOB_PRIORITY_TYPE const getPriority () const override {

			//Earliest deadline first:
			//

			return JOB_MAX_PRIORITY;

			//return (d < ApplicationTime::Since (t_start) ?  : JOB_MIN_PRIORITY);
			//return static_cast<JOB_PRIORITY_TYPE>(std::max((d - ApplicationTime::Since (t_prev)).count (), JOB_MIN_PRIORITY));
		}
	};
}
