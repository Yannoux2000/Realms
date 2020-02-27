#pragma once

/////////////////////////////////////////////////////////////////////////////////////////////
///////////////// Tiago Costa, 2014             
/////////////////////////////////////////////////////////////////////////////////////////////

#include "Allocator.h"

namespace rlms {
	class SmallBlockAllocator : public Allocator
	{
	public:
		SmallBlockAllocator(Allocator& backing_allocator, uint32_t block_size, uint32_t block_alignment);
		~SmallBlockAllocator();

		void* allocate(size_t size, uint8_t alignment) override;
		void deallocate(void* p) override;

		static const uint32_t MIN_BLOCK_SIZE = 4 * 1024; //4kb

	private:

		struct AllocationHeader
		{
			size_t size;
			uint8_t     adjustment;
		};

		struct FreeBlock
		{
			size_t     size;
			FreeBlock* next;
		};

		struct NewBlock
		{
			FreeBlock* block;
			FreeBlock* prev;
		};

		static_assert(sizeof(AllocationHeader) >= sizeof(FreeBlock),
					  "sizeof(AllocationHeader) must be >= sizeof(FreeBlock)");

		SmallBlockAllocator(const SmallBlockAllocator&);
		SmallBlockAllocator& operator=(const SmallBlockAllocator&);

		NewBlock allocateNewBlock();

		FreeBlock* _free_blocks;

		Allocator& _backing_allocator;
		uint32_t        _block_size;
		uint32_t        _block_alignment;
	};
};