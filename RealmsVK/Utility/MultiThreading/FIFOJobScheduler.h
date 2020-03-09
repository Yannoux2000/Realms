#pragma once
#include "IJobScheduler.h"
#include "JobSystem.h"

#include <array>
#include <mutex>

namespace rlms {
	class FIFOJobScheduler : public IJobScheduler {
	private:
		static constexpr size_t size = 256;

		std::array<Job, size> joblist;
		size_t head;
		size_t tail;
		std::mutex lock;
	public:

		FIFOJobScheduler () : head (0), tail (0) {}

		inline bool add_job (Job const& item) override {
			bool ret = false;
			size_t next = (head + 1) % size;
			lock.lock ();

			if (next != tail) {
				joblist[head] = item;
				head = next;
				ret = true;
			}

			lock.unlock ();
			return ret;
		}

		inline void reset () {}

		inline virtual bool elect_job (Job*& item) override {
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
