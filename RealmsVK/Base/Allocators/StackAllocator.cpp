#include "StackAllocator.h"
#include <cassert>

StackAllocator::StackAllocator (void* start, size_t size) : Allocator (start, size), _current_pos (start) {
	assert (size > 0);
}

StackAllocator::~StackAllocator () {
	_current_pos = nullptr;
}

void* StackAllocator::allocate (size_t size, uint8_t alignment) {
	assert (size != 0);
	uint8_t adjustment = pointerMath::alignForwardAdjustmentWithHeader (_current_pos, alignment, sizeof (AllocationHeader));

	if (_used_memory + adjustment + size > _size) return nullptr;

	void* aligned_address = pointerMath::add (_current_pos, adjustment);

	//Add Allocation Header 
	AllocationHeader* header = (AllocationHeader*)(pointerMath::subtract (aligned_address, sizeof (AllocationHeader)));
	header->adjustment = adjustment;

	_current_pos = pointerMath::add (aligned_address, size);
	_used_memory += size + adjustment;
	_num_allocations++;

	return aligned_address;
}

void StackAllocator::deallocate (void* &&p) {
	//assert (p == _prev_position);

	//Access the AllocationHeader in the bytes before p 
	AllocationHeader* header = (AllocationHeader*)(pointerMath::subtract (p, sizeof (AllocationHeader)));
	_used_memory -= (uintptr_t)_current_pos - (uintptr_t)p + header->adjustment;
	_current_pos = pointerMath::subtract (p, header->adjustment);

	p = nullptr;
	_num_allocations--;
}