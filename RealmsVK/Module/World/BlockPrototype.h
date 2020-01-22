#pragma once
#include "../../CoreTypes.h"
#include "../../Base/Math/VoxelMath.h"

#include "Block.h"

#include "../Graphics/IMesh.h"

namespace rlms {
	class BlockPrototype {
	private:
		BLOCK_TYPE_ID m_type_id;
		IMesh* m_mesh;
		bool m_transparency;

	public:
		BlockPrototype (BLOCK_TYPE_ID type_id, IMesh* mesh, bool transparency = false) :
			m_type_id (type_id), m_transparency (transparency), m_mesh (mesh) {};

		BlockPrototype () :
			m_type_id (Block::None), m_transparency (true), m_mesh (nullptr) {};

		void load() {
			m_mesh->import ();
			m_mesh->optimise ();
			m_mesh->load ();
		}

		void create (Block& b) {
			b = Block (m_type_id, m_transparency);
		}

		void update () {

		}

		IMesh* mesh () {
			return m_mesh;
		}
	};
}