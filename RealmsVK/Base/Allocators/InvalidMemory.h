#pragma once
#include "../Exception.h"

namespace rlms {
	class InvalidMemory : public Exception {
	public:
		InvalidMemory (std::string t_type) noexcept : Exception (t_type) {};
		InvalidMemory (std::string t_type, std::string t_description) noexcept : Exception (t_type, t_description) {};
	};

	struct OutOfMemory : public InvalidMemory {
		OutOfMemory () noexcept : InvalidMemory ("Out Of Memory", "Make sure your have allocated sufficient memory for the game.") {}
	};

	struct BadAlloc: public InvalidMemory {
		BadAlloc () noexcept : InvalidMemory ("Bad Allocation", "Make sure your have allocated sufficient RAM for the game.") {}
	};
}
