#pragma once
#include <cstdint>
static const uint8_t DEFAULT_ALIGNMENT = 0;

namespace rlms {
	class IAllocator {
	public:
		IAllocator (void*& start, const size_t& size);
		virtual ~IAllocator ();

		virtual void* allocate (size_t size, uint8_t alignment = DEFAULT_ALIGNMENT) = 0;
		virtual void deallocate (void*&& p) = 0;

		void* getStart () const {
			return _start;
		}
		size_t getSize () const {
			return _size;
		}
		size_t getUsedMemory () const {
			return _used_memory;
		}
		size_t getNumAllocations () const {
			return _num_allocations;
		}

	protected:
		IAllocator (const IAllocator&) = default;
		IAllocator& operator=(IAllocator&) = default;

		void* _start;
		size_t _size;
		size_t _used_memory;
		size_t _num_allocations;
	};
}
#include "IAllocator.inl"

namespace pointerMath {
	bool isAligned (const void* address, uint8_t alignment);
	template<class T> bool isAligned (const T* address);

	void* alignForward (void* address, uint8_t alignment);
	const void* alignForward (const void* address, uint8_t alignment);

	void* alignBackward (void* address, uint8_t alignment);
	const void* alignBackward (const void* address, uint8_t alignment);

	uint8_t alignForwardAdjustment (const void* address, uint8_t alignment);
	template<typename T> uint8_t alignForwardAdjustmentWithHeader (const void* address, uint8_t alignment);
	uint8_t alignBackwardAdjustment (const void* address, uint8_t alignment);

	void* add (void* p, size_t x);
	const void* add (const void* p, size_t x);

	void* subtract (void* p, size_t x);
	const void* subtract (const void* p, size_t x);
}
#include "PointerMath.inl"