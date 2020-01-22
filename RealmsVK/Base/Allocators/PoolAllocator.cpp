#include "PoolAllocator.h" 

#include <cassert>

PoolAllocator::PoolAllocator (size_t objectSize, uint8_t objectAlignment, size_t size, void* mem) : Allocator (mem, size), _objectSize (objectSize), _objectAlignment (objectAlignment) {
	_objectAlignment = objectAlignment;

	//Calculate adjustment needed to keep object correctly aligned 
	uint8_t adjustment = pointerMath::alignForwardAdjustment (mem, objectAlignment);
	_free_list = (void**)pointerMath::add (mem, adjustment);
	size_t numObjects = (size - adjustment) / objectSize;
	void** p = _free_list;

	//Initialize free blocks list 
	for (size_t i = 0; i < numObjects - 1; i++) {
		*p = pointerMath::add (p, objectSize);
		p = (void**)* p;
	}

	*p = nullptr;
}

PoolAllocator::~PoolAllocator () {
	_free_list = nullptr;
}

void* PoolAllocator::allocate (size_t size, uint8_t alignment) {

	if (_free_list == nullptr) return nullptr;
	void* p = _free_list;
	_free_list = (void**)(*_free_list);
	_used_memory += _objectSize;
	_num_allocations++;
	return p;
}

void PoolAllocator::deallocate (void* &&p) {
	*((void**)p) = _free_list;
	_free_list = (void**)p;
	_used_memory -= _objectSize;
	_num_allocations--;
	p = nullptr;
}
