#pragma once
#include "Allocator.h"
#include <vector>

class MasqueradeAllocator : public Allocator {
public:
	MasqueradeAllocator (void* start, const size_t size);
	virtual ~MasqueradeAllocator ();

	void* allocate (size_t size, uint8_t alignment) override;
	void deallocate (void*&& p) override;
private:
	std::vector<void*> _alloc;
};

