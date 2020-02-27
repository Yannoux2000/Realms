#pragma once

/////////////////////////////////////////////////////////////////////////////////////////////
///////////////// Tiago Costa, 2014
/////////////////////////////////////////////////////////////////////////////////////////////

#include "Allocator.h"

namespace rlms {
	class ProxyAllocator : public Allocator
	{
	public:
		ProxyAllocator(Allocator& allocator);
		~ProxyAllocator();

		void* allocate(size_t size, uint8_t alignment) override;

		void deallocate(void* p) override;

	private:
		ProxyAllocator(const ProxyAllocator&);
		ProxyAllocator& operator=(const ProxyAllocator&);

		Allocator& _allocator;
	};
};