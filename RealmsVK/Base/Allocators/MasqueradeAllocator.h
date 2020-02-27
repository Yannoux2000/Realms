#pragma once
#include "Allocator.h"
#include <vector>
namespace rlms {
	class MasqueradeAllocator : public Allocator {
	public:
		MasqueradeAllocator (const size_t size, void* start);
		virtual ~MasqueradeAllocator ();

		void* allocate (size_t size, uint8_t alignment) override;
		void deallocate (void* p) override;
	private:
		std::vector<void*> _alloc;
	};
}
