#include "LinearAllocator.h"

#include <cassert>

using namespace rlms;

LinearAllocator::LinearAllocator(size_t size, void* start)
	: Allocator(size), _start(start), _current_pos(start)
{

}

LinearAllocator::~LinearAllocator () {}

void LinearAllocator::deallocate(void* p)
{
	assert("Cannot call deallocate on Linear Allocators" && false);
}

void* LinearAllocator::getStart() const
{
	return _start;
}

void* LinearAllocator::getMark() const
{
	return _current_pos;
}