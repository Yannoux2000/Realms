#pragma once
#include "../Exception.h"

namespace rlms {
	class InvalidMemory : public Exception {
	public:
		InvalidMemory (std::string t_type) noexcept : Exception (t_type) {};
		InvalidMemory (std::string t_type, std::string t_description) noexcept : Exception (t_type, t_description) {};
	};

	struct BadAlloc : public InvalidMemory {
		BadAlloc () noexcept : InvalidMemory ("Memory Allocation Failed", "Memory Couldn't be allocated for this program, maybe due to a lack of available RAM.") {}
	};

	struct OutOfMemory : public InvalidMemory {
		OutOfMemory () noexcept : InvalidMemory ("Out Of Memory", "Make sure your have allocated sufficient memory for the game.") {}
	};

	struct MemoryOverride : public InvalidMemory {
		MemoryOverride () noexcept : InvalidMemory ("Memory Override", "You are overriding memory already in usage") {}
	};
}
