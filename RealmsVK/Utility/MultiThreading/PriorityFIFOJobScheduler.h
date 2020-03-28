#pragma once

#include <mutex>
#include <array>
#include <vector>
#include <algorithm>

#include "IJobScheduler.h"
#include "JobSystem.h"

namespace rlms {
	class PriorityFIFOJobScheduler : public IJobScheduler {
	private:
		static bool comparator (IJob* const& a, IJob* const& b) {
			return a->getPriority () < b->getPriority (); 
		}
		static constexpr size_t size = 256;

		std::vector<IJob*> joblist;
		std::vector<IJob*> current_p_joblist;
		std::mutex lock;

		size_t tail = 0;

		JOB_PRIORITY_TYPE current_p = JOB_MAX_PRIORITY;

	public:
		inline bool add_job (IJob* const& item) override {
			bool ret = false;
			lock.lock ();

			if (joblist.size() <= size) {
				auto it_head = std::upper_bound (joblist.begin (), joblist.end (), item, comparator);
				joblist.emplace (it_head, std::move(item));
				ret = true;
			}

			lock.unlock ();
			return ret;
		}

		inline void reset () {
			tail = 0;
		}

		inline bool elect_job (IJob*& item) {
			bool ret = false;
			lock.lock ();

			if(current_p_joblist.begin() + tail == current_p_joblist.end()) {
				current_p_joblist.clear ();
				tail = 0;
			}

			if (current_p_joblist.empty()) {
				current_p_joblist.clear ();

				auto it = std::min (joblist.begin (), joblist.end ());

				if (it != joblist.end ()) { //extract same priority jobs to be executed now
					auto begin = std::lower_bound (joblist.begin (), joblist.end (), (*it), comparator);
					auto end = std::upper_bound (begin, joblist.end (), (*it), comparator);

					//transfer jobs to current_priority queue
					current_p_joblist.assign (begin, end);
					joblist.erase (begin, end);

					//non-availables are reinserted back
					for (int i = 0; i < current_p_joblist.size(); i++) {
						auto it = current_p_joblist.begin () + i;

						if (!(*it)->available ()) {
							auto it_head = std::upper_bound (joblist.begin (), joblist.end (), (*it), comparator);
							joblist.emplace (it_head, std::move ((*it)));

							current_p_joblist.erase (it);
							i--;
						}
					}

					//reset iterator
					tail = 0;
				}
			}

			if(current_p_joblist.begin () + tail != current_p_joblist.end()) {
				item = *(current_p_joblist.begin () + tail);
				tail++;
				ret = true;
			}

			lock.unlock ();
			return ret;
		}

		PriorityFIFOJobScheduler () : tail (0), joblist ({}) {}
		~PriorityFIFOJobScheduler () {}
	};
}
