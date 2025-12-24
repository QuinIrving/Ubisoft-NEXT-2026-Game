#pragma once
#include <stdexcept>
#include <cmath>
#include "Vec3.h"
#include "Mat4.h"

#include <Windows.h>

class Quaternion;

template <typename T>
class Vec4 {
public:
	T x{};
	T y{};
	T z{};
	T w{};

public:
	constexpr Vec4() = default;
	constexpr Vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {};
	constexpr Vec4(Vec3<T> v, T w = T(1)) : x(v.x), y(v.y), z(v.z), w(w) {};

	Vec4<T> operator-() const; // negation

	// Vector addition/subtraction
	Vec4<T> operator+(const Vec4<T>& rhs) const;
	Vec4<T>& operator+=(const Vec4<T>& rhs);
	Vec4<T> operator-(const Vec4<T>& rhs) const;
	Vec4<T>& operator-=(const Vec4<T>& rhs);

	// Scalar multiplication
	Vec4<T> operator*(const T scalar) const;
	Vec4<T>& operator*=(const T scalar);
	Vec4<T> operator/(const T scalar) const;
	Vec4<T>& operator/=(const T scalar);

	// To allow Mat4 to be able to do Mat4[row][col] like a typical 2d array
	T& operator[](int col);
	const T& operator[](int col) const;

	// Matrix multiplication
	Vec4<T> operator*(const Mat4<T>& rhs) const;
	Vec4<T>& operator*=(const Mat4<T>& rhs);

	// Quaternion multiplication
	Vec4<T> operator*(const Quaternion& q) const;
	Vec4<T>& operator*=(const Quaternion& q);

	// Scalar power
	Vec4<T> operator^(const T scalar) const;
	Vec4<T>& operator^=(const T scalar);

	// Equality, checks if all elements are equal
	bool operator==(const Vec4<T>& rhs) const;
	bool operator!=(const Vec4<T>& rhs) const;

	constexpr T DotProduct(const Vec4<T>& rhs) const;
	constexpr static T DotProduct(const Vec4<T>& lhs, const Vec4<T>& rhs);

	Vec4<T> GetNormalized() const;
	T GetMagnitude() const;
	T GetMagnitudeSquared() const;

	void Print() const;
};

template <typename T>
Vec4<T> Vec4<T>::operator-() const {
	return Vec4<T>(-x, -y, -z, -w);
}

template <typename T>
Vec4<T> Vec4<T>::operator+(const Vec4<T>& rhs) const {
	return Vec4<T>(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
}

template <typename T>
Vec4<T>& Vec4<T>::operator+=(const Vec4<T>& rhs) {
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	w += rhs.w;

	return *this;
}

template <typename T>
Vec4<T> Vec4<T>::operator-(const Vec4<T>& rhs) const {
	return Vec4<T>(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
}

template <typename T>
Vec4<T>& Vec4<T>::operator-=(const Vec4<T>& rhs) {
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	w -= rhs.w;

	return *this;
}

template <typename T>
Vec4<T> Vec4<T>::operator*(const T scalar) const {
	return Vec4<T>(x * scalar, y * scalar, z * scalar, w * scalar);
}

template <typename T>
Vec4<T>& Vec4<T>::operator*=(const T scalar) {
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;

	return *this;
}

template <typename T>
Vec4<T> Vec4<T>::operator/(const T scalar) const {
	if (scalar == T(0)) {
		throw std::runtime_error("Vec4::operator/ - Attempted to divide by zero");
	}

	return Vec4<T>(x / scalar, y / scalar, z / scalar, w / scalar);
}

template <typename T>
Vec4<T>& Vec4<T>::operator/=(const T scalar) {
	if (scalar == T(0)) {
		throw std::runtime_error("Vec4::operator/= - Attempted to divide by zero");
	}

	x /= scalar;
	y /= scalar;
	z /= scalar;
	w /= scalar;

	return *this;
}

template <typename T>
T& Vec4<T>::operator[](int col) {
	if (col < 0 || col >= 4) {
		throw std::out_of_range("Attempted to access a Vec4 col outside of range");
	}

	return (&x)[col];
}

template <typename T>
const T& Vec4<T>::operator[](int col) const {
	if (col < 0 || col >= 4) {
		throw std::out_of_range("Attempted to access a Vec4 col outside of range");
	}

	return (&x)[col];
}

template <typename T>
Vec4<T> Vec4<T>::operator^(const T scalar) const {
	return Vec4<T>(std::pow(x, scalar), std::pow(y, scalar), std::pow(z, scalar), std::pow(w, scalar));
}

template <typename T>
Vec4<T>& Vec4<T>::operator^=(const T scalar) {
	x = std::pow(x, scalar);
	y = std::pow(y, scalar);
	z = std::pow(z, scalar);
	w = std::pow(w, scalar);

	return *this;
}

template <typename T>
bool Vec4<T>::operator==(const Vec4<T>& rhs) const {
	return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
}

template <typename T>
bool Vec4<T>::operator!=(const Vec4<T>& rhs) const {
	return !(*this == rhs);
}

template <typename T>
constexpr T Vec4<T>::DotProduct(const Vec4<T>& rhs) const {
	return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
}

template <typename T>
constexpr T Vec4<T>::DotProduct(const Vec4<T>& lhs, const Vec4<T>& rhs) {
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

template <typename T>
Vec4<T> Vec4<T>::GetNormalized() const {
	T mag = this->GetMagnitude();

	if (mag == 0) {
		throw std::runtime_error("Vec4::GetNormalized - Attempted to divide by a zero magnitude");
	}

	return Vec4<T>(x / mag, y / mag, z / mag, w / mag);
}

template <typename T>
T Vec4<T>::GetMagnitude() const {
	return std::sqrt(x * x + y * y + z * z + w * w);
}

template <typename T>
T Vec4<T>::GetMagnitudeSquared() const {
	return x * x + y * y + z * z + w * w;
}

template <typename T>
void Vec4<T>::Print() const {
	char buffer[128];
	std::snprintf(buffer, sizeof(buffer), "[ %.2f, %.2f, %.2f, %.2f ]\n", x, y, z, w);
	OutputDebugString(buffer);
}