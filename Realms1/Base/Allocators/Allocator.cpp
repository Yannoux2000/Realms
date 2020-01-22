#include "Allocator.h"
#include <cassert>

Allocator::Allocator(void*& start,const size_t& size) :_start(start), _size (size), _used_memory (0), _num_allocations (0) {
	assert (size > 0);
}

Allocator::~Allocator () {
	//_size = 0;
}

void* Allocator::getStart () const {
	return _start;
}

size_t Allocator::getSize () const {
	return _size;
}

size_t Allocator::getUsedMemory () const {
	return _used_memory;
}

size_t Allocator::getNumAllocations () const {
	return _num_allocations;
}