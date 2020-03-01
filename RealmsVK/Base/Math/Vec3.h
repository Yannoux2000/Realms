#pragma once
#include <array>

#include "../../CoreTypes.h"
#include "Vec.h"

#include <glm/glm.hpp>

namespace rlms {
	template <typename T> class Vec3 : public Vec<T, 3> {
	public:
		Vec3 () : Vec<T, 3> ({ 0,0,0 }) {};
		Vec3 (T x, T y, T z) : Vec<T, 3> ({ x, y ,z }) {}
		Vec3 (std::array<T, 3> v) : Vec<T, 3> (v) {}
		Vec3 (std::initializer_list<T> v) : Vec<T, 3> (v) {}
		template <glm::qualifier q> Vec3 (glm::vec<3, T, q> const& v) : Vec<T, 3> ({ v.x, v.y, v.z }) {}
		
		inline T dot (Vec3 const& v) {
			return (this->operator[](0) * v[0] + this->operator[](1) * v[1] + this->operator[](2) * v[2]);
		}

		T& getX () {
			return this->operator[](0);
		}

		T& getY () {
			return this->operator[](1);
		}

		T& getZ () {
			return this->operator[](2);
		}

		inline Vec3& cross (Vec3 const& v) {
			Vec3<T> ret (
				this->operator[](1) * v[2] - v[2] * this->operator[](1),
				this->operator[](2) * v[0] - v[0] * this->operator[](2),
				this->operator[](0) * v[1] - v[1] * this->operator[](0)
			);
			return ret;
		}

		inline double norm () {
			return sqrt (dot (*this));
		}

		inline Vec3<double>& direction () {
			Vec3<double> v (
				this->operator[](0) / norm (),
				this->operator[](1) / norm (),
				this->operator[](2) / norm ()
			);
			return v;
		}

		inline std::array<T, 4> & homogeneous () {
			std::array<T, 4> v = { this->operator[](0), this->operator[](1), this->operator[](2), 1 };
			return v;
		}

		template<typename T2> operator Vec3<T2> () const {
			Vec3<T2> v (
				static_cast<T2>(this->operator[](0)),
				static_cast<T2>(this->operator[](1)),
				static_cast<T2>(this->operator[](2))
			);
			return v;
		}

		operator glm::vec<3, T> () const {
			glm::vec<3, T> v (this->operator[](0), this->operator[](1), this->operator[](2));
			return v;
		}
	};
	
	//#include "Vec3.inl"

	/*
	typedef Vec3<CHUNK_COORDS_TYPE> ChunkCoords3;
	typedef Vec3<COORDS_TYPE> Coords3;
	typedef Vec3<double> Vec3d;
	typedef Vec3<float> Vec3f;
	*/

}