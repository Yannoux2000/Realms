#pragma once

/////////////////////////////////////////////////////////////////////////////////////////////
///////////////// Tiago Costa, 2014             
/////////////////////////////////////////////////////////////////////////////////////////////

#include "Allocator.h"
#include "../../_Preprocess.h"

namespace rlms {
	class FreeListAllocator : public Allocator
	{
	public:
		FreeListAllocator(size_t size, void* start);
		~FreeListAllocator();

		void* allocate(size_t size, uint8_t alignment) override;
		void deallocate(void* p) override;

#ifdef RLMS_DEBUG
		void checkFreeBlockList();
#endif

	private:

		struct AllocationHeader
		{
			#ifdef RLMS_DEBUG
			size_t identifier;
			uint32_t	   magic_number;
			#endif
			size_t size;
			uint8_t     adjustment;
		};

		struct FreeBlock
		{
			#ifdef RLMS_DEBUG
			uint32_t magic_number;
			#endif
			size_t     size;
			FreeBlock* next;
		};

		static_assert(sizeof(AllocationHeader) >= sizeof(FreeBlock), "sizeof(AllocationHeader) < sizeof(FreeBlock)");

		FreeListAllocator(const FreeListAllocator&);
		FreeListAllocator& operator=(const FreeListAllocator&);

		FreeBlock* _free_blocks;

		#ifdef RLMS_DEBUG
		size_t _next_identifier;
		#endif
	};
};