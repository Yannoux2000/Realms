#pragma once

#include <mutex>
#include <array>
#include <vector>
#include <algorithm>

#include "JobSystem.h"

namespace rlms {
	class PriorityFIFOJobScheduler {
	private:
		static constexpr size_t size = 256;

		std::vector<Job> joblist;
		std::vector<Job> current_p_joblist;
		std::mutex lock;

		size_t tail = 0;

		std::vector<Job>::iterator it_head;

		JOB_PRIORITY_TYPE current_p = JOB_MAX_PRIORITY;

	public:
		inline bool add_job (Job const& item) {
			bool ret = false;
			lock.lock ();

			if (joblist.size() <= size) {
				it_head = std::lower_bound (joblist.begin (), joblist.end (), item);
				joblist.insert (it_head, item);
				ret = true;
			}

			lock.unlock ();
			return ret;
		}

		inline void reset () {
			tail = 0;
		}

		inline bool elect_job (Job*& item) {
			bool ret = false;
			lock.lock ();

			if (current_p_joblist.empty()) {
				current_p_joblist.clear ();

				auto it = std::min (joblist.begin (), joblist.end ());

				if (it != joblist.end ()) { //extract same priority jobs to be executed now
					auto begin = std::lower_bound (joblist.begin (), joblist.end (), (*it), [](Job const& a, Job const& b) { return a.priority < b.priority; });
					auto end = std::upper_bound (begin, joblist.end (), (*it), [](Job const& a, Job const& b) { return a.priority < b.priority; });

					//transfer jobs to current_priority queue
					current_p_joblist.assign (begin, end);
					joblist.erase (begin, end);

					//reset iterator
					tail = 0;
				}
			}

			if(current_p_joblist.begin () + tail != current_p_joblist.end()) {
				item = &(*(current_p_joblist.begin () + tail));
				tail++;
				ret = true;
			}else {
				current_p_joblist.clear ();
				tail = 0;
			}

			lock.unlock ();
			return ret;
		}

		PriorityFIFOJobScheduler () : tail (0), joblist ({}) {}
		~PriorityFIFOJobScheduler () {}
	};
}
