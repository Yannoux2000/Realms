#include "Vec3.h"
#include <cmath>

using namespace rlms;

//template<typename T>inline T Vec3<T>::dot (Vec3 const& v) {
//	return this->_v[0] * v._v[0] + this->_v[1] * v._v[1] + this->_v[2] * v._v[2];
//}
//
//template<typename T> inline Vec3<T>& Vec3<T>::cross (Vec3 const& v) {
//	Vec3<T> ret (
//		this->_v[1] * v._v[2] - v._v[2] * this->_v[1],
//		this->_v[2] * v._v[0] - v._v[0] * this->_v[2],
//		this->_v[0] * v._v[1] - v._v[1] * this->_v[0]
//	);
//	return ret;
//}
//
//template<typename T> inline double Vec3<T>::norm () {
//	return sqrt (dot (*this));
//}
//
//template<typename T> inline Vec3<double>& Vec3<T>::direction () {
//	Vec3<double> v (
//		this->_v[0] / norm (),
//		this->_v[1] / norm (),
//		this->_v[2] / norm ()
//	);
//	return v;
//}
//
//template<typename T> inline std::array<T, 4> & Vec3<T>::homogeneous () {
//	std::array<T, 4> v = { this->_v[0], this->_v[1], this->_v[2], 1 };
//	return v;
//}
//
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//
//template<typename T> inline Vec3<T>& Vec3<T>::operator+(Vec3 const& other) {
//	Vec3 v (
//		this->_v[0] + other._v[0],
//		this->_v[1] + other._v[1],
//		this->_v[2] + other._v[2]
//	);
//	return v;
//}
//
//template<typename T> inline Vec3<T>& Vec3<T>::operator-(Vec3 const& other) {
//	Vec3 v (
//		this->_v[0] - other._v[0],
//		this->_v[1] - other._v[1],
//		this->_v[2] - other._v[2]
//	);
//	return v;
//}
//
//template<typename T> inline Vec3<T>& Vec3<T>::operator*(Vec3 const& other) {
//	Vec3 v (
//		this->_v[0] * other._v[0],
//		this->_v[1] * other._v[1],
//		this->_v[2] * other._v[2]
//	);
//	return v;
//}
//
//template<typename T> inline Vec3<T>& Vec3<T>::operator/(Vec3 const& other) {
//	Vec3 v (
//		this->_v[0] / other._v[0],
//		this->_v[1] / other._v[1],
//		this->_v[2] / other._v[2]
//	);
//	return v;
//}
//
//template<typename T> inline Vec3<T>& Vec3<T>::operator+(T const& other) {
//	Vec3 v (
//		this->_v[0] + other,
//		this->_v[1] + other,
//		this->_v[2] + other
//	);
//	return v;
//}
//
//template<typename T> inline Vec3<T>& Vec3<T>::operator-(T const& other) {
//	Vec3 v (
//		this->_v[0] - other,
//		this->_v[1] - other,
//		this->_v[2] - other
//	);
//	return v;
//}
//
//template<typename T> inline Vec3<T>& Vec3<T>::operator*(T const& other) {
//	Vec3 v (
//		this->_v[0] * other,
//		this->_v[1] * other,
//		this->_v[2] * other
//	);
//	return v;
//}
//
//template<typename T> inline Vec3<T>& Vec3<T>::operator/(T const& other) {
//	Vec3 v (
//		this->_v[0] / other,
//		this->_v[1] / other,
//		this->_v[2] / other
//	);
//	return v;
//}
//
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//
//template<typename T> inline Vec3<T>& Vec3<T>::operator+=(Vec3 const& other) {
//	Vec3 v (
//		this->_v[0] + other._v[0],
//		this->_v[1] + other._v[1],
//		this->_v[2] + other._v[2]
//	);
//	return v;
//}
//
//template<typename T> inline Vec3<T>& Vec3<T>::operator-=(Vec3 const& other) {
//	Vec3 v (
//		this->_v[0] - other._v[0],
//		this->_v[1] - other._v[1],
//		this->_v[2] - other._v[2]
//	);
//	return v;
//}
//
//template<typename T> inline Vec3<T>& Vec3<T>::operator*=(Vec3 const& other) {
//	Vec3 v (
//		this->_v[0] * other._v[0],
//		this->_v[1] * other._v[1],
//		this->_v[2] * other._v[2]
//	);
//	return v;
//}
//
//template<typename T> inline Vec3<T>& Vec3<T>::operator/=(Vec3 const& other) {
//	Vec3 v (
//		this->_v[0] / other._v[0],
//		this->_v[1] / other._v[1],
//		this->_v[2] / other._v[2]
//	);
//	return v;
//}
//
//template<typename T> inline Vec3<T>& Vec3<T>::operator+=(T const& other) {
//	Vec3 v (
//		this->_v[0] + other,
//		this->_v[1] + other,
//		this->_v[2] + other
//	);
//	return v;
//}
//
//template<typename T> inline Vec3<T>& Vec3<T>::operator-=(T const& other) {
//	Vec3 v (
//		this->_v[0] - other,
//		this->_v[1] - other,
//		this->_v[2] - other
//	);
//	return v;
//}
//
//template<typename T> inline Vec3<T>& Vec3<T>::operator*=(T const& other) {
//	Vec3 v (
//		this->_v[0] * other,
//		this->_v[1] * other,
//		this->_v[2] * other
//	);
//	return v;
//}
//
//template<typename T> inline Vec3<T>& Vec3<T>::operator/=(T const& other) {
//	Vec3 v (
//		this->_v[0] / other,
//		this->_v[1] / other,
//		this->_v[2] / other
//	);
//	return v;
//}