#pragma once

#include <cstdint>
#include <limits>
#include <chrono>

namespace rlms {
	using MEMORY_SIZE_TYPE = unsigned long long;

	using DURATION = std::chrono::microseconds;

	using OWNER_ID_TYPE = uint8_t;

	using JOB_PRIORITY_TYPE = uint16_t;
	constexpr JOB_PRIORITY_TYPE JOB_MIN_PRIORITY = std::numeric_limits<JOB_PRIORITY_TYPE>::max ();
	constexpr JOB_PRIORITY_TYPE JOB_MAX_PRIORITY = std::numeric_limits<JOB_PRIORITY_TYPE>::min () + 1;

	using JOB_RATE_TYPE = uint16_t;

	using ENTITY_ID = uint16_t;
	using COMPONENT_ID = uint16_t;
	using COMPONENT_TYPE_ID = uint16_t;
	using EVENT_ID = uint16_t;

	using INPUT_ADDRESS_SUB_TYPE = uint8_t;

	using GAME_TICK_TYPE = uint16_t;
	using BLOCK_TYPE_ID = uint16_t;

	using IMESH_TYPE_ID = uint16_t;

	using BLOCK_COORDS_TYPE = uint16_t;
	using CHUNK_COORDS_TYPE = int32_t;
}