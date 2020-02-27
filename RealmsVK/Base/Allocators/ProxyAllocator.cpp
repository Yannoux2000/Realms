#include "ProxyAllocator.h"
#include <cassert>

using namespace rlms;

ProxyAllocator::ProxyAllocator(Allocator& allocator)
	: Allocator(allocator.getSize()), _allocator(allocator)
{

}

ProxyAllocator::~ProxyAllocator()
{

}

void* ProxyAllocator::allocate(size_t size, uint8_t alignment)
{
	assert(size != 0);

	_num_allocations++;

	size_t mem = _allocator.getUsedMemory();

	void* p = _allocator.allocate(size, alignment);

	_used_memory += _allocator.getUsedMemory() - mem;

	return p;
}

void ProxyAllocator::deallocate(void* p)
{
	_num_allocations--;

	size_t mem = _allocator.getUsedMemory();

	_allocator.deallocate(p);

	_used_memory -= mem - _allocator.getUsedMemory();
}