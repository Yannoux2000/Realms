#pragma once
#include "IAllocator.h"
#include <vector>

namespace rlms{
	class MasqueradeAllocator : public IAllocator {
	public:
		MasqueradeAllocator (void* start, const size_t& size);
		~MasqueradeAllocator ();

		void* allocate (size_t size, uint8_t alignment = DEFAULT_ALIGNMENT) override;
		void deallocate (void*&& p) override;

	private:
		std::vector<void*> _allocs;
	};

}
