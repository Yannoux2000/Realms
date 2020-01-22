#pragma once
#include <iostream>

namespace rlms{
	constexpr double M_PI = 3.14159265359;
	constexpr double M_CULLING = 0.3;
	constexpr int CHUNK_DIM = 16;

	extern constexpr std::ostream* MAIN_OUTPUT = &(std::cout);
}
