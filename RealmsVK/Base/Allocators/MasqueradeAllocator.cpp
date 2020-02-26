#include "MasqueradeAllocator.h"
#include "InvalidMemory.h"

MasqueradeAllocator::MasqueradeAllocator (void* start, const size_t size) : Allocator (start, size) {}

MasqueradeAllocator::~MasqueradeAllocator () {
	for (auto it = _alloc.begin(); it != _alloc.end(); it++) {
		free (*it);
	}
}

void* MasqueradeAllocator::allocate (size_t size, uint8_t alignment) {
	void* mem = malloc (size);
	if (mem != nullptr) {
		_alloc.push_back (mem);
		return mem;
	}
	throw rlms::BadAlloc ();
}

void MasqueradeAllocator::deallocate (void*&& p) {
	auto it = _alloc.begin ();
	while (it != _alloc.end()) {
		if (*it == p) {
			free (p);
			return;
		}
		it++;
	}
}
