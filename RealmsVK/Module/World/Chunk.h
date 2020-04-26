#pragma once
#include "../../Constants.h"
#include "../../Base/Math/VoxelMath.h"
#include "Block.h"
#include "BlockRegister.h"

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include <array>

namespace rlms {
	struct Chunk : public IVoxel {
		std::array < std::array < std::array < Block, CHUNK_DIM>, CHUNK_DIM>, CHUNK_DIM> m_blocks;
		glm::vec3 origin;

		//debugging
		Chunk () : IVoxel (), m_blocks (), origin () {}

		void create_sample () {
			BlockPrototype* pta = BlockRegister::Get (3);
			BlockPrototype* ptb = BlockRegister::Get (4);
			for (int z = 0; z < CHUNK_DIM; z++) {
				for (int y = 0; y < CHUNK_DIM; y++) {
					for (int x = 0; x < CHUNK_DIM; x++) {
						if (z == 8 && ((x/2)+(y/2))%2) {
							pta->create (m_blocks[x][y][z]);
						} else if (z > 4 && z < 8) {
							ptb->create (m_blocks[x][y][z]);
						}
					}
				}
			}
		}

		void optimize () {
			auto blocks = std::vector<std::vector<std::vector<IVoxel>>> 
				(CHUNK_DIM, std::vector<std::vector<IVoxel>> 
				(CHUNK_DIM, std::vector<IVoxel> 
				(CHUNK_DIM, IVoxel ())));

			for (int z = 0; z < CHUNK_DIM; z++) {
				for (int y = 0; y < CHUNK_DIM; y++) {
					for (int x = 0; x < CHUNK_DIM; x++) {
						blocks[x][y][z].culling = m_blocks[x][y][z].culling;
					}
				}
			}

			VoxelMath::OcclusionCulling (blocks);

			for (int z = 0; z < CHUNK_DIM; z++) {
				for (int y = 0; y < CHUNK_DIM; y++) {
					for (int x = 0; x < CHUNK_DIM; x++) {
						m_blocks[x][y][z].culling = blocks[x][y][z].culling;
					}
				}
			}
		}

		//void render () {
		//	float pas = 8;

		//	gr->use ();
		//	glm::mat4 trans = glm::mat4 (1.0f);

		//	BLOCK_TYPE_ID prev = Block::None;

		//	for (int z = 0; z < CHUNK_DIM; z++) {
		//		for (int y = 0; y < CHUNK_DIM; y++) {
		//			for (int x = 0; x < CHUNK_DIM; x++) {

		//				Block &current = m_blocks[x][y][z];
		//				if(Block::isValid(current) && IVoxel::HasAny (current, IVoxel::Faces)){
		//					if(current.type_id != prev){
		//						gr->bind (BlockRegister::Get (current.type_id)->mesh());
		//					}

		//					trans = glm::translate (glm::mat4(1.0f), origin);
		//					trans = glm::translate (trans, glm::vec3 (pas * x, pas * y, pas * z));

		//					gr->setModelTrans (trans);
		//					gr->draw ();

		//				}
		//			}
		//		}
		//	}
		//	IMesh::Unbind ();
		//}

		~Chunk() {}

	};
}


