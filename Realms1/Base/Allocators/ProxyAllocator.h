#pragma once
#include "Allocator.h"

class ProxyAllocator : public Allocator {
public:
	ProxyAllocator (void*& start, const size_t& size);
	virtual ~ProxyAllocator ();

	void* allocate (size_t size, uint8_t alignment) override;
	void deallocate (void*&& p) override;
};

