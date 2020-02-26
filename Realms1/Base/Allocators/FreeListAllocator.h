#pragma once
#include "Allocator.h"
class FreeListAllocator_old : public Allocator {
public:

	FreeListAllocator_old (void* start, size_t size);
	~FreeListAllocator_old ();

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

	FreeListAllocator_old (const FreeListAllocator_old&);
	//Prevent copies because it might cause errors 
	FreeListAllocator_old& operator=(const FreeListAllocator_old&) = delete;
	FreeBlock* _free_blocks;
};
