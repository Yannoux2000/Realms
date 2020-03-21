#pragma once
#include "JobSystem.h"

namespace rlms {
	class IJobScheduler {
	public:
		inline virtual bool add_job (IJob* const& item) {
			return false;
		}

		inline void reset() {}

		inline virtual bool elect_job (IJob*& item) {
			return false;
		}
	};
}
