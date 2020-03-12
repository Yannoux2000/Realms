#pragma once

#include <cstdint>
#include <limits>
 class ratio;

namespace rlms {
	template<std::uintmax_t Num, std::uintmax_t Denom = 1> class Ratio {
	private:
		constexpr double decimal = num / dem;

	public:
		constexpr std::uintmax_t num = Num;
		constexpr std::uintmax_t den = Denom;

		inline static std::uintmax_t int () {
			return decimal;
		}
	};
}

