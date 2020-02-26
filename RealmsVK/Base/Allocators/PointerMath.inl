#include <cstdint>
#include "IAllocator.h"

//Inline Definitions
namespace pointerMath {
	inline bool isAligned (const void* address, uint8_t alignment) {
		return alignForwardAdjustment (address, alignment) == 0;
	}

	template<class T>
	inline bool isAligned (const T* address) {
		return alignForwardAdjustment (address, __alignof(T)) == 0;
	}

	template<typename T>
	uint8_t alignForwardAdjustmentWithHeader (const void* address, uint8_t alignment) {
		return uint8_t ();
	}

	inline void* alignForward (void* address, uint8_t alignment) {
		return (void*)((reinterpret_cast<uintptr_t>(address) + static_cast<uintptr_t>(alignment - 1)) & static_cast<uintptr_t>(~(alignment - 1)));
	}

	inline const void* alignForward (const void* address, uint8_t alignment) {
		return (void*)((reinterpret_cast<uintptr_t>(address) + static_cast<uintptr_t>(alignment - 1)) & static_cast<uintptr_t>(~(alignment - 1)));
	}

	inline void* alignBackward (void* address, uint8_t alignment) {
		return (void*)(reinterpret_cast<uintptr_t>(address) & static_cast<uintptr_t>(~(alignment - 1)));
	}

	inline const void* alignBackward (const void* address, uint8_t alignment) {
		return (void*)(reinterpret_cast<uintptr_t>(address) & static_cast<uintptr_t>(~(alignment - 1)));
	}

	inline uint8_t alignForwardAdjustment (const void* address, uint8_t alignment) {
		uint8_t adjustment = alignment - (reinterpret_cast<uintptr_t>(address) & static_cast<uintptr_t>(alignment - 1));

		if (adjustment == alignment)
			return 0; //already aligned

		return adjustment;
	}

	inline uint8_t alignForwardAdjustmentWithHeader (const void* address,uint8_t alignment, const uint8_t& neededSpace) {
		if (neededSpace > alignment)
			alignment = neededSpace;

		return neededSpace + alignForwardAdjustment (pointerMath::add (address, neededSpace), alignment);
	}

	inline uint8_t alignBackwardAdjustment (const void* address, uint8_t alignment) {
		uint8_t adjustment = reinterpret_cast<uintptr_t>(address) & static_cast<uintptr_t>(alignment - 1);

		if (adjustment == alignment)
			return 0; //already aligned

		return adjustment;
	}

	inline void* add (void* p, size_t x) {
		return (void*)(reinterpret_cast<uintptr_t>(p) + x);
	}

	inline const void* add (const void* p, size_t x) {
		return (const void*)(reinterpret_cast<uintptr_t>(p) + x);
	}

	inline void* subtract (void* p, size_t x) {
		return (void*)(reinterpret_cast<uintptr_t>(p) - x);
	}

	inline const void* subtract (const void* p, size_t x) {
		return (const void*)(reinterpret_cast<uintptr_t>(p) - x);
	}
}