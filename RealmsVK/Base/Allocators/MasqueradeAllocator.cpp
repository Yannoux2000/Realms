#include "MasqueradeAllocator.h"
#include "InvalidMemory.h"

using namespace rlms;

MasqueradeAllocator::MasqueradeAllocator (void* start, const size_t& size) : IAllocator(start, size) {

}

MasqueradeAllocator::~MasqueradeAllocator () {
	for (auto  it = _allocs.begin(); it != _allocs.end(); it++) {
		free (*it);
	}
}

void* MasqueradeAllocator::allocate (size_t size, uint8_t alignment) {
	void* mem = malloc (size);
	if (mem != nullptr) {
		_allocs.push_back (mem);
		return mem;
	}
	throw BadAlloc (); //can happend
}

void MasqueradeAllocator::deallocate (void*&& p) {
	auto it = _allocs.begin ();
	while (it != _allocs.end()) {
		if (*it == p) break;
		it++;
	}

	_allocs.erase (it);
	free (p);
	p = nullptr;
}
