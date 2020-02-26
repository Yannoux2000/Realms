#include "ProxyAllocator.h"
#include "InvalidMemory.h"

using namespace rlms;

ProxyAllocator::ProxyAllocator (void*& start, const size_t& size) : IAllocator(start, size) {}

ProxyAllocator::ProxyAllocator (ProxyAllocator&& other) noexcept : IAllocator (other._start, other._size) { }

ProxyAllocator::~ProxyAllocator () {}

void* ProxyAllocator::allocate (size_t size, uint8_t alignment) {
	if (_num_allocations != 0) throw MemoryOverride();

	_size = size;
	_used_memory = size + alignment;
	_num_allocations = 1;
	return _start;
}

void ProxyAllocator::deallocate (void*&& p) {
	_num_allocations = 0;
	_size = _used_memory = 0;
	p = nullptr;
}

ProxyAllocator ProxyAllocator::Malloc (const size_t size) {
	void* mem = malloc (size);
	if (mem != nullptr) return ProxyAllocator (mem, size);
	throw BadAlloc (); //can happend
}

void ProxyAllocator::Realloc (ProxyAllocator old_alloc, const size_t new_size) {
	old_alloc._num_allocations = 0;
	old_alloc._size = old_alloc._used_memory = 0;
}
