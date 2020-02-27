#pragma once

/////////////////////////////////////////////////////////////////////////////////////////////
///////////////// Tiago Costa, 2014             
/////////////////////////////////////////////////////////////////////////////////////////////

#include <cstdint>

namespace rlms
{
	//Declarations
	namespace pointer_math
	{
		bool        isAligned(const void* address, uint8_t alignment);

		template<class T>
		bool		isAligned(const T* address);

		void*       alignForward(void* address, uint8_t alignment);
		const void* alignForward(const void* address, uint8_t alignment);

		void*       alignBackward(void* address, uint8_t alignment);
		const void* alignBackward(const void* address, uint8_t alignment);

		uint8_t          alignForwardAdjustment(const void* address, uint8_t alignment);
		//uint8_t          alignForwardAdjustmentWithHeader(const void* address, uint8_t alignment, uint8_t header_size);
		template<typename T>
		uint8_t			alignForwardAdjustmentWithHeader(const void* address, uint8_t alignment);

		uint8_t          alignBackwardAdjustment(const void* address, uint8_t alignment);

		void*       add(void* p, size_t x);
		const void* add(const void* p, size_t x);

		void*       subtract(void* p, size_t x);
		const void* subtract(const void* p, size_t x);
	}

	//Inline Definitions
	namespace pointer_math
	{
		inline bool isAligned(const void* address, uint8_t alignment)
		{
			return alignForwardAdjustment(address, alignment) == 0;
		}

		template<class T>
		inline bool isAligned(const T* address)
		{
			return alignForwardAdjustment(address, __alignof(T)) == 0;
		}

		inline void* alignForward(void* address, uint8_t alignment)
		{
			return (void*)((reinterpret_cast<uintptr_t>(address)+static_cast<uintptr_t>(alignment - 1)) & static_cast<uintptr_t>(~(alignment - 1)));
		}

		inline const void* alignForward(const void* address, uint8_t alignment)
		{
			return (void*)((reinterpret_cast<uintptr_t>(address)+static_cast<uintptr_t>(alignment - 1)) & static_cast<uintptr_t>(~(alignment - 1)));
		}

		inline void* alignBackward(void* address, uint8_t alignment)
		{
			return (void*)(reinterpret_cast<uintptr_t>(address)& static_cast<uintptr_t>(~(alignment - 1)));
		}

		inline const void* alignBackward(const void* address, uint8_t alignment)
		{
			return (void*)(reinterpret_cast<uintptr_t>(address)& static_cast<uintptr_t>(~(alignment - 1)));
		}

		inline uint8_t alignForwardAdjustment(const void* address, uint8_t alignment)
		{
			uint8_t adjustment = alignment - (reinterpret_cast<uintptr_t>(address)& static_cast<uintptr_t>(alignment - 1));

			if(adjustment == alignment)
				return 0; //already aligned

			return adjustment;
		}
		/*
		inline uint8_t alignForwardAdjustmentWithHeader(const void* address, uint8_t alignment, uint8_t header_size)
		{
			uint8_t adjustment = alignForwardAdjustment(address, alignment);

			uint8_t needed_space = header_size;

			if(adjustment < needed_space)
			{
				needed_space -= adjustment;

				//Increase adjustment to fit header
				adjustment += alignment * (needed_space / alignment);

				if(needed_space % alignment > 0)
					adjustment += alignment;
			}

			return adjustment;
		}
		*/
		template<typename T>
		inline uint8_t alignForwardAdjustmentWithHeader(const void* address, uint8_t alignment)
		{
			if(__alignof(T) > alignment)
				alignment = __alignof(T);

			uint8_t adjustment = sizeof(T) + alignForwardAdjustment(pointer_math::add(address, sizeof(T)), alignment);

			return adjustment;
		}

		inline uint8_t alignBackwardAdjustment(const void* address, uint8_t alignment)
		{
			uint8_t adjustment = reinterpret_cast<uintptr_t>(address)& static_cast<uintptr_t>(alignment - 1);

			if(adjustment == alignment)
				return 0; //already aligned

			return adjustment;
		}

		inline void* add(void* p, size_t x)
		{
			return (void*)(reinterpret_cast<uintptr_t>(p)+x);
		}

		inline const void* add(const void* p, size_t x)
		{
			return (const void*)(reinterpret_cast<uintptr_t>(p)+x);
		}

		inline void* subtract(void* p, size_t x)
		{
			return (void*)(reinterpret_cast<uintptr_t>(p)-x);
		}

		inline const void* subtract(const void* p, size_t x)
		{
			return (const void*)(reinterpret_cast<uintptr_t>(p)-x);
		}
	}
}