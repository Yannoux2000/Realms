#pragma once

#include <mutex>
#include <array>

namespace rlms {

	template<typename T>
	struct JobDescription {

		uint16_t priority = std::numeric_limits<uint16_t>::max ();
		T job;

		JobDescription (const T& item) : job (item) {}
	};

	template <typename T, size_t size> class JobSequencer {
	private:
		std::array<JobDescription<T>, size> joblist;
		std::mutex lock;

		size_t head = 0;
		size_t tail = 0;

	public:
		inline bool add_job (const T& item) {
			bool ret = false;
			lock.lock ();
			size_t next = (head + 1) % size;

			if (next != tail) {
				joblist[head] = item;
				head = next;
				result = true;
			}

			lock.unlock ();
			return ret;
		}

		inline void sequence () {

		}

		inline bool elect_job (const T& item) {
			bool ret = false;
			lock.lock ();

			if (tail != head) {
				item = data[tail];
				tail = (tail + 1) % capacity;
				result = true;
			}

			lock.unlock ();
			return ret;
		}
	};
}
