#pragma once

#include <mutex>
#include <array>

#include "JobSystem.h"

namespace rlms {
	template <size_t size> class JobSequencer {
	private:
		std::array<JobDescription, size> joblist;
		std::mutex lock;

		size_t head = 0;
		size_t tail = 0;

	public:
		inline bool add_job (JobDescription const& item) {
			bool ret = false;
			lock.lock ();
			size_t next = (head + 1) % size;

			if (next != tail) {
				joblist[head] = item;
				head = next;
				ret = true;
			}

			lock.unlock ();
			return ret;
		}

		inline void sequence () {

		}

		inline bool elect_job (JobDescription* item) {
			bool ret = false;
			lock.lock ();

			if (tail != head) {
				item = &joblist[tail];
				tail = (tail + 1) % size;
				ret = true;
			}

			lock.unlock ();
			return ret;
		}
	};
}
