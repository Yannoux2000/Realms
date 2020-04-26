#include "Vec.h"

template<typename T, size_t S> Vec<T, S>::Vec () : _v () {};

template<typename T, size_t S> Vec<T, S>::Vec (std::array<T, S> v) : _v (v) {};

template<typename T, size_t S> Vec<T, S>::Vec (std::initializer_list<T> v) : _v () {
	int i = 0;
	if (v.size () == S) {
		for (auto it = v.begin (); it < v.end (); it++) {
			_v[i++] = *it;
		}
	}
}

template<typename T, size_t S> inline bool Vec<T, S>::operator==(std::array<T, S> v) {
	bool flag = true;
	int i = 0;
	if (v.size () == S) {
		for (int i = 0; i < v.size (); i++) {
			flag &= _v[i] == v[i];
		}
	}
	return flag;
}

template<typename T, size_t S> inline T& Vec<T, S>::operator[](size_t i) {
	return _v[i];
}

template<typename T, size_t S> inline T Vec<T, S>::operator[](size_t i) const {
	return _v[i];
}

template<typename T, size_t S> inline bool Vec<T, S>::operator==(Vec<T, S> v) {
	bool flag = true;
	int i = 0;
	if (v.size () == S) {
		for (int i = 0; i < v.size (); i++) {
			flag &= _v[i] == v[i];
		}
	}
	return flag;
}

template<typename T, size_t S>
inline T Vec<T, S>::dot (Vec const& other) {
	T sum = 0;
	for (int i = 0; i < S; i++) {
		sum += _v[i] * other._v[i];
	}
	return sum;
}

template<typename T, size_t S>
inline Vec<T, S>& Vec<T, S>::operator+(T const& other) {
	Vec<T, S> ret;
	for (int i = 0; i < S; i++) {
		ret._v[i] = _v[i] + other;
	}
	return ret;
}

template<typename T, size_t S>
inline Vec<T, S>& Vec<T, S>::operator-(T const& other) {
	Vec<T, S> ret;
	for (int i = 0; i < S; i++) {
		ret._v[i] = _v[i] - other;
	}
	return ret;
}

template<typename T, size_t S>
inline Vec<T, S>& Vec<T, S>::operator*(T const& other) {
	Vec<T, S> ret;
	for (int i = 0; i < S; i++) {
		ret._v[i] = _v[i] * other;
	}
	return ret;
}

template<typename T, size_t S>
inline Vec<T, S>& Vec<T, S>::operator/(T const& other) {
	Vec<T, S> ret;
	for (int i = 0; i < S; i++) {
		ret._v[i] = _v[i] / other;
	}
	return ret;
}

template<typename T, size_t S>
inline Vec<T, S> & Vec<T, S>::operator+=(T const& other) {
	for (int i = 0; i < S; i++) {
		_v[i] += other;
	}
	return (*this);
}

template<typename T, size_t S>
inline Vec<T, S>& Vec<T, S>::operator-=(T const& other) {
	for (int i = 0; i < S; i++) {
		_v[i] -= other;
	}
	return (*this);
}

template<typename T, size_t S>
inline Vec<T, S>& Vec<T, S>::operator*=(T const& other) {
	for (int i = 0; i < S; i++) {
		_v[i] *= other;
	}
	return (*this);
}

template<typename T, size_t S>
inline Vec<T, S>& Vec<T, S>::operator/=(T const& other) {
	for (int i = 0; i < S; i++) {
		_v[i] /= other;
	}
	return (*this);
}

template<typename T, size_t S>
inline Vec<T, S>& Vec<T, S>::operator+(Vec<T, S> const& other) {
	Vec<T, S> ret;
	for (int i = 0; i < S; i++) {
		ret._v[i] = _v[i] + other._v[i];
	}
	return ret;
}

template<typename T, size_t S>
inline Vec<T, S>& Vec<T, S>::operator-(Vec<T, S> const& other) {
	Vec<T, S> ret;
	for (int i = 0; i < S; i++) {
		ret._v[i] = _v[i] - other._v[i];
	}
	return ret;
}

template<typename T, size_t S>
inline Vec<T, S>& Vec<T, S>::operator*(Vec<T, S> const& other) {
	Vec<T, S> ret;
	for (int i = 0; i < S; i++) {
		ret._v[i] = _v[i] * other._v[i];
	}
	return ret;
}

template<typename T, size_t S>
inline Vec<T, S>& Vec<T, S>::operator/(Vec<T, S> const& other) {
	Vec<T, S> ret;
	for (int i = 0; i < S; i++) {
		ret._v[i] = _v[i] / other._v[i];
	}
	return ret;
}

template<typename T, size_t S>
inline Vec<T, S>& Vec<T, S>::operator+=(Vec<T, S> const& other) {
	for (int i = 0; i < S; i++) {
		_v[i] += other._v[i];
	}
	return (*this);
}

template<typename T, size_t S>
inline Vec<T, S>& Vec<T, S>::operator-=(Vec<T, S> const& other) {
	for (int i = 0; i < S; i++) {
		_v[i] -= other._v[i];
	}
	return (*this);
}

template<typename T, size_t S>
inline Vec<T, S>& Vec<T, S>::operator*=(Vec<T, S> const& other) {
	for (int i = 0; i < S; i++) {
		_v[i] *= other._v[i];
	}
	return (*this);
}

template<typename T, size_t S>
inline Vec<T, S>& Vec<T, S>::operator/=(Vec<T, S> const& other) {
	for (int i = 0; i < S; i++) {
		if (other._v[i] == 0) {
			throw std::overflow_error ("Math error : Tried to divide by 0.");
		}

		_v[i] /= other._v[i];
	}
	return (*this);
}
