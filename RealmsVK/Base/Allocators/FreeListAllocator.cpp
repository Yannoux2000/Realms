#include "FreeListAllocator.h"

#include "InvalidMemory.h"

#include <cassert>

FreeListAllocator::FreeListAllocator (void* start, size_t size) : Allocator (start, size), _free_blocks ((FreeBlock*)start) {
	assert (size > sizeof (FreeBlock));
	_free_blocks->size = size;
	_free_blocks->next = nullptr;
}

FreeListAllocator::~FreeListAllocator () {
	_free_blocks = nullptr;
}

void* FreeListAllocator::allocate (size_t size, uint8_t alignment) {
	assert (size != 0 && alignment != 0);
	FreeBlock* prev_free_block = nullptr;
	FreeBlock* free_block = _free_blocks;
//freeblock->size = size left in the whole freelist

	while (free_block != nullptr) { //while whe didn't reached the end of the linked list
		//Calculate adjustment needed to keep object correctly aligned 
		uint8_t adjustment = pointerMath::alignForwardAdjustmentWithHeader (free_block, alignment, sizeof (AllocationHeader));
		size_t total_size = size + adjustment;

		//if block is too slim
		if (free_block->size - total_size <= sizeof (AllocationHeader)) {
			prev_free_block = free_block;
			free_block = free_block->next;
			continue;
		}

		static_assert(sizeof (AllocationHeader) >= sizeof (FreeBlock), "sizeof(AllocationHeader) < sizeof(FreeBlock)");

		//if block is fat enouth or not taken (if taken, then total_size = free_block->size)
		if (free_block->size > total_size) {
			FreeBlock* next_block = (FreeBlock*)(pointerMath::add (free_block, total_size));

			next_block->size = free_block->size - (total_size + sizeof(AllocationHeader));
			next_block->next = free_block->next;

			if (prev_free_block != nullptr) {
				prev_free_block->next = next_block;
			} else {
				free_block->next = next_block;
				free_block->size = (total_size + sizeof (AllocationHeader));
			}
		} else if (free_block->next == nullptr) {
			throw rlms::OutOfMemory();
			return nullptr;
		}

		if(free_block!= nullptr) {
			uintptr_t aligned_address = (uintptr_t)free_block + adjustment;
			AllocationHeader* header = (AllocationHeader*)(aligned_address - sizeof (AllocationHeader));
			header->size = total_size;
			header->adjustment = adjustment;
			_used_memory += total_size;
			_num_allocations++;

			assert (pointerMath::alignForwardAdjustment ((void*)aligned_address, alignment) == 0);

			return (void*)aligned_address;
		}
	}

	//ASSERT(false && "Couldn't find free block large enough!"); 
	return nullptr;
}

void FreeListAllocator::deallocate (void*&& p) {
	assert (p != nullptr);

	AllocationHeader* header = (AllocationHeader*)pointerMath::subtract (p, sizeof (AllocationHeader));
	uintptr_t  block_start = reinterpret_cast<uintptr_t>(p) - header->adjustment;
	size_t block_size = header->size;
	uintptr_t  block_end = block_start + block_size;

	FreeBlock* prev_free_block = nullptr;
	FreeBlock* free_block = _free_blocks;

	//find block used
	while (free_block != nullptr) {
		//we entered the concerned memory block
		if ((uintptr_t)free_block >= block_end) break;
		prev_free_block = free_block;
		free_block = free_block->next;
	}

	//if block is first
	if (prev_free_block == nullptr) {
		prev_free_block = (FreeBlock*)block_start;
		prev_free_block->size = block_size;
		prev_free_block->next = _free_blocks;
		_free_blocks = prev_free_block;
	//took previous block
	} else if ((uintptr_t)prev_free_block + prev_free_block->size == block_start) {
		prev_free_block->size += block_size;

	//creating block at the address
	} else {
		FreeBlock* temp = (FreeBlock*)block_start;
		temp->size = block_size;
		temp->next = prev_free_block->next;
		prev_free_block->next = temp;
		prev_free_block = temp;
	}

	//if we found the block in our allocation space, and it's the right one
	if (free_block != nullptr && (uintptr_t)free_block == block_end) {
		//fuse previous block with this one so the previous one can allocate at it
		prev_free_block->size += free_block->size;
		prev_free_block->next = free_block->next;
	}

	//update info
	_num_allocations--;
	_used_memory -= block_size;
	p = nullptr;
}
