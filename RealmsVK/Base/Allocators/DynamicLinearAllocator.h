#pragma once

/////////////////////////////////////////////////////////////////////////////////////////////
///////////////// Tiago Costa, 2015
/////////////////////////////////////////////////////////////////////////////////////////////

#include "LinearAllocator.h"

/*
	TODO: Remove getStart() from LinearAllocator
		because it doesn't make sense in DynamicLinearAllocators and there's no real need for it
			
*/

namespace rlms
{
	class Allocator;

	class DynamicLinearAllocator : public LinearAllocator
	{
	public:
		DynamicLinearAllocator(Allocator& backing_allocator, size_t block_size, uint8_t block_alignment);
		~DynamicLinearAllocator() override final;

		void* allocate(size_t size, uint8_t alignment = DEFAULT_ALIGNMENT) override final;

		void rewind(void* mark) override final;

		void clear() override final;

	private:
		DynamicLinearAllocator(const DynamicLinearAllocator&);
		DynamicLinearAllocator& operator=(const DynamicLinearAllocator&);

		bool allocateNewBlock(size_t size, uint8_t alignment);

		struct BlockDesc;

		Allocator& _backing_allocator;

		BlockDesc* _current_block;

		size_t _block_size;
		uint8_t     _block_alignment;
	};
};