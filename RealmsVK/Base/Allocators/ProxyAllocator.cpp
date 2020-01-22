#include "ProxyAllocator.h"
#include <cassert>

ProxyAllocator::ProxyAllocator (void*& start, const size_t& size) : Allocator(start, size) {
}

ProxyAllocator::~ProxyAllocator () {}

void* ProxyAllocator::allocate (size_t size, uint8_t alignment) {
	uint8_t adjustment = pointerMath::alignForwardAdjustment (_start, alignment);

	if (_used_memory + adjustment + size > _size) return nullptr;

	uintptr_t  aligned_address = (uintptr_t)_start + adjustment;
	_used_memory += size + adjustment;
	_num_allocations++;

	return (void*)aligned_address;
}

void ProxyAllocator::deallocate (void*&& p) {
	_used_memory = 0;
	_num_allocations = 0;
	p = nullptr;
}
