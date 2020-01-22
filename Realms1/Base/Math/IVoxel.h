#pragma once
#include "../../CoreTypes.h"

namespace rlms {
	struct IVoxel {
		//flag to represent either or not a face is coverred
		//0 = covered

		//hidden overrides all the axis flags, if hidden is true all flags are overritten to not displayed

		static constexpr uint8_t Xp = 0b00000001; //flag representing X axis positive side
		static constexpr uint8_t Xn = 0b00000010; //X negative
		static constexpr uint8_t Yp = 0b00000100; //Y
		static constexpr uint8_t Yn = 0b00001000;
		static constexpr uint8_t Zp = 0b00010000; //Z
		static constexpr uint8_t Zn = 0b00100000;

		static constexpr uint8_t Faces = 0b00111111;

		static constexpr uint8_t Transparent = 0b01000000; //flag for when the block is see-through
		static constexpr uint8_t Hidden = 0b10000000; //flag for when the block is not displayed

		uint8_t culling;
		IVoxel () : culling (Transparent) {};
		IVoxel (uint8_t t_culling) : culling (t_culling) {};

		static inline bool isTransparent (IVoxel const& v) {
			return HasAll (v, Transparent);
		}

		static inline void BitSet (IVoxel& v, uint8_t const flags) {
			v.culling |= flags;
		}

		static inline void BitReset (IVoxel& v, uint8_t flags) {
			v.culling &= ~flags;
		}

		static inline void Mask (IVoxel& v, uint8_t const mask) {
			v.culling &= mask;
		}

		static inline bool HasAll (IVoxel const& v, uint8_t const flags) {
			return (v.culling & flags) == flags;
		}

		static inline bool HasAny (IVoxel const& v, uint8_t const flags) {
			return (v.culling & flags);
		}
	};
}