#include "MasqueradeAllocator.h"
#include "InvalidMemory.h"

using namespace rlms;

MasqueradeAllocator::MasqueradeAllocator (const size_t size, void* start) : Allocator (size), _start(start) {}

MasqueradeAllocator::~MasqueradeAllocator () {
	for (auto it = _alloc.begin(); it != _alloc.end(); it++) {
		free (*it);
	}
}

void* MasqueradeAllocator::allocate (size_t size, uint8_t alignment) {
	void* mem = malloc (size);
	if (mem != nullptr) {
		_num_allocations += 1;
		_alloc.push_back (mem);
		return mem;
	}
	throw BadAlloc ();
}

void MasqueradeAllocator::deallocate (void* p) {
	//done at the end

	//auto it = _alloc.begin ();
	//while (it != _alloc.end()) {
	//	if (*it == p) {
	//		free (*it);
	//		_num_allocations -= 1;
	//		_alloc.erase (it);
	//		return;
	//	}
	//	it++;
	//}
	//throw OutOfMemory ();
}
