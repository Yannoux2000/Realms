#pragma once
#include "Allocator.h"
class FreeListAllocator : public Allocator {
public:

	FreeListAllocator (void* start, size_t size);
	~FreeListAllocator ();

	void* allocate (size_t size, uint8_t alignment) override;
	void deallocate (void*&& p) override;

private:

	struct AllocationHeader {
		size_t size;
		uint8_t adjustment;
	};

	struct FreeBlock {
		size_t size; 
		FreeBlock* next;
	};

	FreeListAllocator (const FreeListAllocator&);
	//Prevent copies because it might cause errors 
	FreeListAllocator& operator=(const FreeListAllocator&) = delete;
	FreeBlock* _free_blocks;
};
