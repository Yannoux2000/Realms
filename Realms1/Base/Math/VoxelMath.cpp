#include "VoxelMath.h"

#include <iostream>
#include <bitset>

using namespace rlms;

uint8_t rlms::VoxelMath::BackfacesCulling (glm::vec3 cameraPos) {

	uint8_t backfaces_Culling = IVoxel::Hidden | IVoxel::Transparent; //to avoid overriding this flag

	if (cameraPos.x > -M_CULLING) {
		backfaces_Culling |= IVoxel::Xn;
	}
	if (cameraPos.x < M_CULLING) {
		backfaces_Culling |= IVoxel::Xp;
	}

	if (cameraPos.y > -M_CULLING) {
		backfaces_Culling |= IVoxel::Yn;
	}
	if (cameraPos.y < M_CULLING) {
		backfaces_Culling |= IVoxel::Yp;
	}

	if (cameraPos.z > -M_CULLING) {
		backfaces_Culling |= IVoxel::Zn;
	}
	if (cameraPos.z < M_CULLING) {
		backfaces_Culling |= IVoxel::Zp;
	}

	//std::cout << "( " << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << ")"
	//	<< std::bitset<8> (backfaces_Culling) << std::endl;

	return backfaces_Culling;
}

void rlms::VoxelMath::OcclusionCulling (std::vector<std::vector<std::vector<IVoxel>>>& chunk, uint8_t flags) {
	for (size_t x = 0; x < chunk.size (); x++) {
		for (size_t y = 0; y < chunk[x].size (); y++) {
			for (size_t z = 0; z < chunk[x][y].size (); z++) {

				IVoxel::BitSet (chunk[x][y][z], IVoxel::Faces);

				if (x + 1L < chunk.size ()) {
					IVoxel::BitReset (chunk[x][y][z], (IVoxel::isTransparent (chunk[x + 1L][y][z]) ? 0 : IVoxel::Xp));
				}
				if (x > 0) {
					IVoxel::BitReset (chunk[x][y][z], (IVoxel::isTransparent (chunk[x - 1][y][z]) ? 0 : IVoxel::Xn));
				}

				if (y + 1L < chunk[x].size ()) {
					IVoxel::BitReset (chunk[x][y][z], (IVoxel::isTransparent (chunk[x][y + 1L][z]) ? 0 : IVoxel::Yp));
				}
				if (y > 0) {
					IVoxel::BitReset (chunk[x][y][z], (IVoxel::isTransparent (chunk[x][y - 1][z]) ? 0 : IVoxel::Yn));
				}

				if (z + 1L < chunk[x][y].size ()) {
					IVoxel::BitReset (chunk[x][y][z], (IVoxel::isTransparent (chunk[x][y][z + 1L]) ? 0 : IVoxel::Zp));
				}
				if (z > 0) {
					IVoxel::BitReset (chunk[x][y][z], (IVoxel::isTransparent (chunk[x][y][z - 1]) ? 0 : IVoxel::Zn));
				}

				/*
				if (!IVoxel::HasAll(chunk[x][y][z], IVoxel::Faces)) {
					//std::cout << "ok" << std::endl;
					IVoxel::BitSet(chunk[x][y][z], IVoxel::Hidden);
				}
				*/

				//std::cout << std::bitset<8> (old) << " => " << std::bitset<8> (chunk[x][y][z].culling) << std::endl;
			}
		}
	}
}
