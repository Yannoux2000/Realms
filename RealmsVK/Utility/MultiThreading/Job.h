#pragma once

#include "../../CoreTypes.h"
#include "../../Base/IBase.h"
#include <functional>

namespace rlms {
	// This works fine as f1 and f2 are std::function<void()>
	// binding to a ref will make the evolutions be taken into account
	//
	//void print (int i) {
	//	std::cout << i << '\n';
	//}
	//
	//int main () {
	//	int i = 10;
	//
	//	auto f1 = std::bind (print, i);
	//	auto f2 = std::bind (print, std::ref (i));
	//
	//	i = 20;
	//
	//	f1 ();
	//	f2 ();
	//}

	struct IJob : public IBase {
		std::function<void ()> job;

		IJob () {}
		IJob (std::function<void ()> item ) : job(item) {}
		IJob (IJob && j) noexcept : job(std::move(j.job)) {}

		virtual void operator()() = 0;

		virtual bool available () {
			return true;
		}

		virtual JOB_PRIORITY_TYPE const getPriority () const {
			return JOB_MIN_PRIORITY;
		}

		inline bool operator<(IJob const& other) {
			return getPriority() < other.getPriority ();
		}

		inline bool operator>(IJob const& other) {
			return getPriority () > other.getPriority ();
		}
	};

	class Job : public IJob {
	private:
		JOB_PRIORITY_TYPE priority;

	public:
		Job () : IJob(), priority (JOB_MIN_PRIORITY) {}
		Job (std::function<void ()> item, JOB_PRIORITY_TYPE const n = JOB_MIN_PRIORITY) : IJob (item), priority (n) {}
		Job (Job&& j) noexcept : IJob (std::move (j)), priority(std::move(j.priority))  {}

		void operator()() override {
			if (job) {
				job ();
			} else throw std::exception ("invalid job");
		}

		JOB_PRIORITY_TYPE const getPriority () const override {
			return priority;
		}
	};
}
