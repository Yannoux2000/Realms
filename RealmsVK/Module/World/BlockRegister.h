#pragma once

#include "../../CoreTypes.h"

#include "Block.h"
#include "BlockPrototype.h"

#include <map>

namespace rlms{
	class BlockRegister {
	private:
		static std::map<BLOCK_TYPE_ID, BlockPrototype> m_register;
	public:

		static void Register (BLOCK_TYPE_ID type_id, IMesh* mesh, bool transparency = false) {
			m_register.try_emplace (type_id, type_id, mesh, transparency);
		}

		static BlockPrototype* Get (BLOCK_TYPE_ID const& type_id) {
			return &m_register[type_id];
		}
	};
}
