#pragma once

#include "../../CoreTypes.h"
#include <functional>

namespace rlms {
	struct Job {
		JOB_PRIORITY_TYPE priority;
		std::function<void (void*)> job;
		void* args;

		Job () : args (nullptr), priority (JOB_MIN_PRIORITY) {}

		Job (std::function<void ()> item, JOB_PRIORITY_TYPE const n = JOB_MIN_PRIORITY, void* const& data = nullptr) : args (data), priority (n) {
			job = [item](void* _discated) {item (); };
		}

		Job (std::function<void (void*)> item, JOB_PRIORITY_TYPE const n = JOB_MIN_PRIORITY, void* const& data = nullptr) : job (item), args (data), priority (n) {}

		virtual void operator()() {
			if (job) {
				job (args);
			}
			else throw std::exception ("invalid job");
		}

		virtual bool available () {
			return true;
		}

		inline bool operator<(Job const& other) {
			return priority < other.priority;
		}

		inline bool operator>(Job const& other) {
			return priority > other.priority;
		}
	};

	struct PeriodicJob : public Job {

		PeriodicJob (
			std::function<void ()> const& item,
			JOB_PRIORITY_TYPE const n = JOB_MIN_PRIORITY,
			void* const& data = nullptr
		) : Job(item, n, data) { }


		void operator()() override {
			if (job != nullptr) {
				job (args);
			}
			else throw std::exception ("invalid job");
		}
	};
}
