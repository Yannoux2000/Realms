#pragma once

#include "Allocator.h"

class PoolAllocator : public Allocator {
public:

	PoolAllocator (size_t objectSize, uint8_t objectAlignment, size_t size, void* mem);
	~PoolAllocator ();
	void* allocate (size_t size, uint8_t alignment) override;
	void deallocate (void*&& p) override;

private:

	//Prevent copies because it might cause errors 
	PoolAllocator (const PoolAllocator&);
	PoolAllocator& operator=(const PoolAllocator&) = delete;
	size_t _objectSize;

	uint8_t _objectAlignment;
	void** _free_list;
};

