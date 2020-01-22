#pragma once
#include <array>

#include "../../CoreTypes.h"
#include "Vec.h"

#include "glm/vec2.hpp"

namespace rlms {
	template <typename T> class Vec2 : public Vec<T,2> {
	public:
		Vec2 () : Vec<T, 2> ({ 0,0 }) {};
		Vec2 (T x, T y) : Vec<T, 2> ({ x, y }) {}
		Vec2 (std::array<T, 2> v) : Vec<T, 2> (v) {}
		Vec2 (std::initializer_list<T> v) : Vec<T, 2> (v) {}
		template <glm::qualifier q> Vec2 (glm::vec<2, T, q> const& v) : Vec<T, 2> ({ v.x, v.y }) {}
		Vec2 (Vec<T, 2> const& v) : Vec<T, 2> ({ v[0], v[1] }) {}
	};

	typedef Vec2<int> Vec2i;
}
