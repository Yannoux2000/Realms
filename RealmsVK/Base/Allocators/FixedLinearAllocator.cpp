#include "FixedLinearAllocator.h"
#include <cassert>

using namespace rlms;

FixedLinearAllocator::FixedLinearAllocator(size_t size, void* start) : LinearAllocator(size, start)
{

}

FixedLinearAllocator::~FixedLinearAllocator()
{
}

void* FixedLinearAllocator::allocate(size_t size, uint8_t alignment)
{
	assert(size != 0 && alignment != 0);

	uint8_t adjustment = pointer_math::alignForwardAdjustment(_current_pos, alignment);

	if(_used_memory + adjustment + size > _size)
		return nullptr;

	uintptr_t aligned_address = (uintptr_t)_current_pos + adjustment;

	_current_pos = (void*)(aligned_address + size);

	_used_memory = (uintptr_t)_current_pos - (uintptr_t)_start;

	return (void*)aligned_address;
}

void FixedLinearAllocator::rewind(void* p)
{
	assert(_current_pos > p && _start < p);

	_current_pos = p;
	_used_memory = (uintptr_t)_current_pos - (uintptr_t)_start;
}

void FixedLinearAllocator::clear()
{
	_num_allocations = 0;
	_used_memory     = 0;

	_current_pos = _start;
}