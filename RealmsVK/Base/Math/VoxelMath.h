#pragma once
#include "../../CoreTypes.h"
#include "../../Constants.h"
#include "IVoxel.h"

#include "glm/glm.hpp"
#include <vector>

namespace rlms {
	class VoxelMath {
	public:
		static uint8_t BackfacesCulling (glm::vec3 cameraPos);
		static void OcclusionCulling (std::vector<std::vector<std::vector<IVoxel>>>& chunk, uint8_t flags = IVoxel::Faces);
	};
}
