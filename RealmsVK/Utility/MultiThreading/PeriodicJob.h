#pragma once
#include "Job.h"
#include "JobSystem.h"
#include "../../Base/Timer/ApplicationTime.h"
#include "../../CoreTypes.h"

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

		PeriodicJob () : IJob (), t_start (ApplicationTime::Now ()), t_end(ApplicationTime::Now()) {
			t_end += std::chrono::duration_cast<ApplicationTime::chrono::duration>(d);
		}
		PeriodicJob (std::function<void ()> item) : IJob (item), t_start (ApplicationTime::Now ()), t_end (ApplicationTime::Now ()) {
			t_end += std::chrono::duration_cast<ApplicationTime::chrono::duration>(d);
		}
		PeriodicJob (std::function<void ()>&& item, ApplicationTime::time_point t_start) : IJob (item), t_start (t_start), t_end (t_start) {
			t_end += std::chrono::duration_cast<ApplicationTime::chrono::duration>(d);
		}
		PeriodicJob (PeriodicJob && j) noexcept : IJob(std::move(j)), t_start(std::move(j.t_start)), t_end (std::move(j.t_end)) {}

		void operator()() override {
			if (job != nullptr) {
				t_start = ApplicationTime::Now ();
				job ();
			} else throw std::exception ("invalid job");

			rlms::JobSystem::Register (PeriodicJob<period> (std::move(job), t_end));
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
