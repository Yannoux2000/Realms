#include "IAllocator.h"
#include <cassert>

using namespace rlms;

rlms::IAllocator::IAllocator (void*& start, const size_t& size) : _start (start), _size (size), _used_memory (0), _num_allocations (0) {
	assert (size > 0);
}

rlms::IAllocator::~IAllocator () {
	_size = 0;
	_start = nullptr;
}