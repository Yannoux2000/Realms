#pragma once
#include "JobSystem.h"
#include <memory>

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
