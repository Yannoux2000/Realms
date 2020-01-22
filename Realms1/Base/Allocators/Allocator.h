#pragma once
#include <cstdint>
static const uint8_t DEFAULT_ALIGNMENT = 8;

class Allocator {
public:
	Allocator (void*& start, const size_t& size);
	virtual ~Allocator ();

	virtual void* allocate (size_t size, uint8_t alignment = DEFAULT_ALIGNMENT) = 0;
	virtual void deallocate (void*&& p) = 0;

	void* getStart () const;
	size_t getSize () const;
	size_t getUsedMemory () const;
	size_t getNumAllocations () const;

protected:
	Allocator (const Allocator&) = default;
	Allocator& operator=(Allocator&) = default;

	void* _start;
	size_t _size;
	size_t _used_memory;
	size_t _num_allocations;
};

namespace allocator {
	template <class T, class... Args>
	T* allocateNew (Allocator& allocator, Args&& ... args) {
		return new (allocator.allocate (sizeof (T), __alignof(T))) T (std::forward<Args> (args)...);
	}

	template <class T>
	void deallocateDelete (Allocator& allocator, T*& object) {
		object->~T ();
		allocator.deallocate (static_cast<void*&&>(object));
		object = nullptr;
	}

	template <class T>
	T* allocateArray (Allocator& allocator, size_t length) {
		//assert (length != 0);

		uint8_t header_size = sizeof (size_t) / sizeof (T);

		if (sizeof (size_t) % sizeof (T) > 0)
			header_size += 1;

		//Allocate extra space to store array length in the bytes before the array
		T* p = static_cast<T*>(allocator.allocate (sizeof (T) * (length + header_size), __alignof(T))) + header_size;

		*((static_cast<size_t*>(p)) - 1) = length;

		for (size_t i = 0; i < length; i++)
			new (&p[i]) T;

		return p;
	}

	template <class T>
	T* allocateArrayNoConstruct (Allocator& allocator, size_t length) {
		//assert (length != 0);

		uint8_t header_size = sizeof (size_t) / sizeof (T);

		if (sizeof (size_t) % sizeof (T) > 0)
			header_size += 1;

		//Allocate extra space to store array length in the bytes before the array
		T* p = static_cast<T*>(allocator.allocate (sizeof (T) * (length + header_size), __alignof(T))) + header_size;

		*(static_cast<size_t*>(p) - 1) = length;

		return p;
	}

	template <class T>
	void deallocateArray (Allocator& allocator, T* array) {
		ASSERT (array != nullptr);

		size_t length = *(static_cast<size_t*>(array) - 1);

		//should be reverse order
		for (size_t i = 0; i < length; i++)
			array[i].~T ();

		//Calculate how much extra memory was allocated to store the length before the array
		uint8_t header_size = sizeof (size_t) / sizeof (T);

		if (sizeof (size_t) % sizeof (T) > 0)
			header_size += 1;

		allocator.deallocate (array - header_size);
	}

	template <class T>
	void deallocateArrayNoDestruct (Allocator& allocator, T* array) {
		ASSERT (array != nullptr);

		//Calculate how much extra memory was allocated to store the length before the array
		uint8_t header_size = sizeof (size_t) / sizeof (T);

		if (sizeof (size_t) % sizeof (T) > 0)
			header_size += 1;

		allocator.deallocate (array - header_size);
	}
};

namespace pointerMath {
	bool        isAligned (const void* address, uint8_t alignment);

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
