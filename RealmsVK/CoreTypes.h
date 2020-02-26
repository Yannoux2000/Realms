#pragma once

#include <cstdint>

namespace rlms {
	using MEMORY_SIZE_TYPE = unsigned long long;

	using OWNER_ID_TYPE = uint8_t;

	using ENTITY_ID = uint16_t;
	using COMPONENT_ID = uint16_t;
	using EVENT_ID = uint16_t;

	using GAME_TICK_TYPE = uint16_t;
	using BLOCK_TYPE_ID = uint16_t;

	using IMESH_TYPE_ID = uint16_t;

	using BLOCK_COORDS_TYPE = uint16_t;
	using CHUNK_COORDS_TYPE = int32_t;
}