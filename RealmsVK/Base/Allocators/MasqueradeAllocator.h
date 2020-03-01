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

		void* getStart () {
			return _start;
		}

	private:
		std::vector<void*> _alloc;
		void* _start;
	};
}
