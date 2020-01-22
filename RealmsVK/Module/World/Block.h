#pragma once
#include "../../CoreTypes.h"
#include "../../Base/Math/VoxelMath.h"

namespace rlms {
	struct Block : public IVoxel {
		static constexpr BLOCK_TYPE_ID None = 0; //error case.
		static constexpr BLOCK_TYPE_ID Air = 1; //default empty.
		static constexpr BLOCK_TYPE_ID Special = 2; //used to fill chunk if used by a special entity.

		Block (BLOCK_TYPE_ID tid = None, bool transparent = true) : IVoxel ((transparent ? IVoxel::Transparent : 0)), type_id(tid) {}

		static bool isValid (Block& b) {
			return b.type_id != None;
		}

		BLOCK_TYPE_ID type_id;
	};
}