#pragma once
#include <iomanip>

namespace rlms {
	template<typename T, size_t S> class Vec {
	protected:
		std::array<T, S> _v;
	public:
		Vec ();
		Vec (std::array<T, S> v);
		Vec (std::initializer_list<T> v);

		inline bool operator==(std::array<T, S> v);
		inline bool operator==(Vec<T, S> v);

		inline T& operator[](size_t i);
		inline T operator[](size_t i) const;
		inline T dot (Vec const&);

		inline Vec& operator+(T const&);
		inline Vec& operator-(T const&);
		inline Vec& operator*(T const&);
		inline Vec& operator/(T const&);

		inline Vec& operator+=(T const&);
		inline Vec& operator-=(T const&);
		inline Vec& operator*=(T const&);
		inline Vec& operator/=(T const&);

		inline Vec& operator+(Vec const&);
		inline Vec& operator-(Vec const&);
		inline Vec& operator*(Vec const&);
		inline Vec& operator/(Vec const&);

		inline Vec& operator+=(Vec const&);
		inline Vec& operator-=(Vec const&);
		inline Vec& operator*=(Vec const&);
		inline Vec& operator/=(Vec const&);

		friend std::ostream& operator << (std::ostream& l, Vec<T, S> const& vec) {
			l << "(";
			for (int i = 0; i < S; i++) {
				if (i != 0) l << ",";
				l << std::setw (4) << vec[i];
			}
			l << ")";
			return l;
		}
	};


	template<typename T, size_t S> Vec<T, S>& operator+(T const& left, Vec<T, S>& right) {
		return right + left;
	}

	template<typename T, size_t S> Vec<T, S>& operator-(T const& left, Vec<T, S>& right) {
		return right - left;
	}

	template<typename T, size_t S> Vec<T, S>& operator*(T const& left, Vec<T, S>& right) {
		return right * left;
	}

	#include "Vec.inl"
}