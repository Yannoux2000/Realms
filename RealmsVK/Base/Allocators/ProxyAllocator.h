#pragma once
#include "IAllocator.h"

namespace rlms {
	class ProxyAllocator : public IAllocator {
	public:
		ProxyAllocator (void*& start, const size_t& size);
		ProxyAllocator (ProxyAllocator&& other) noexcept;
		~ProxyAllocator ();

		void* allocate (size_t size, uint8_t alignment = DEFAULT_ALIGNMENT) override;
		void deallocate (void*&& p) override;

		ProxyAllocator& operator=(ProxyAllocator&&) = default;
		static ProxyAllocator Malloc (const size_t size);
		static void Realloc (ProxyAllocator old_alloc, const size_t new_size);
	};
}
