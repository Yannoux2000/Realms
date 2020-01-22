#pragma once
#include <array>

#include "../../CoreTypes.h"
#include "Vec.h"

#include "SFML/System/Vector2.hpp"
#include "glm/vec2.hpp"

namespace rlms {
	template <typename T> class Vec2 : public Vec<T,2> {
	public:
		Vec2 () : Vec<T, 2> ({ 0,0 }) {};
		Vec2 (T x, T y) : Vec<T, 2> ({ x, y }) {}
		Vec2 (std::array<T, 2> v) : Vec<T, 2> (v) {}
		Vec2 (std::initializer_list<T> v) : Vec<T, 2> (v) {}
		Vec2 (sf::Vector2<T> const& v) : Vec<T, 2> ({ v.x, v.y }) {}
		template <glm::qualifier q> Vec2 (glm::vec<2, T, q> const& v) : Vec<T, 2> ({ v.x, v.y }) {}

		T& getX () {
			return this->operator*[0];
		}

		T& getY () {
			return this->operator*[1];
		}
	};
}