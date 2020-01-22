#pragma once
#include "Allocator.h"
class LinearAllocator : public Allocator {
public:
	LinearAllocator (void* start,const size_t size);
	~LinearAllocator ();

	void* allocate (size_t size, uint8_t alignment) override;
	void deallocate (void*&& p) override;
	void clear ();

private:
	//Prevent copies because it might cause errors 
	LinearAllocator (const LinearAllocator&);
	LinearAllocator& operator=(const LinearAllocator&) = delete;
	void* _current_pos;
};

