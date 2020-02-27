#pragma once

/////////////////////////////////////////////////////////////////////////////////////////////
///////////////// Tiago Costa, 2015             
/////////////////////////////////////////////////////////////////////////////////////////////

#include "BlockAllocator.h"
#include "ProxyAllocator.h"

#include "../../_Preprocess.h"

namespace rlms {
#ifdef RLMS_DEBUG
	class BlockAllocatorProxy : public ProxyAllocator
	{
	public:
		BlockAllocatorProxy(BlockAllocator& allocator);
		~BlockAllocatorProxy() override;

		size_t getBlockSize() const;
		uint8_t getAlignment() const;

	private:

		BlockAllocatorProxy(const BlockAllocatorProxy&);
		BlockAllocatorProxy& operator=(const BlockAllocatorProxy&);

		BlockAllocator& _allocator;
	};
#endif
};

#ifdef RLMS_DEBUG
	#define NEW_BLOCK_ALLOCATOR_PROXY(alloc, block_allocator) \
		allocator::allocateNew<BlockAllocatorProxy>(alloc, block_allocator)
#else
	#define NEW_BLOCK_ALLOCATOR_PROXY(alloc, block_allocator) &block_allocator
#endif