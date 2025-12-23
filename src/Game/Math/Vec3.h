#pragma once
#include <stdexcept>
#include <cmath>

class Quaternion;

template <typename T>
class Vec4;

template <typename T>
class Vec3{
public:
	T x{};
	T y{};
	T z{};

public:
	constexpr Vec3() = default;
	constexpr Vec3(T x, T y, T z) : x(x), y(y), z(z) {};
	constexpr Vec3(Vec4<T> v) : x(v.x), y(v.y), z(v.z) {};

	Vec3<T> operator-() const; // negation

	// Scalar addition/subtraction
	Vec3<T> operator+(const T scalar) const;
	Vec3<T>& operator+=(const T scalar);
	Vec3<T> operator-(const T scalar) const;
	Vec3<T>& operator-=(const T scalar);

	// Vector addition/subtraction
	Vec3<T> operator+(const Vec3<T>& rhs) const;
	Vec3<T>& operator+=(const Vec3<T>& rhs);
	Vec3<T> operator-(const Vec3<T>& rhs) const;
	Vec3<T>& operator-=(const Vec3<T>& rhs);

	// Scalar multiplication
	Vec3<T> operator*(const T scalar) const;
	Vec3<T>& operator*=(const T scalar);
	Vec3<T> operator/(const T scalar) const;
	Vec3<T>& operator/=(const T scalar);

	// Vector multiplication
	Vec3<T> operator*(const Vec3<T>& rhs) const;
	Vec3<T>& operator*=(const Vec3<T>& rhs);
	Vec3<T> operator/(const Vec3<T>& rhs) const;
	Vec3<T>& operator/=(const Vec3<T>& rhs);

	// Quaternion multiplication
	//Vec3<T> operator*(const Quaternion& q) const;
	//Vec3<T>& operator*=(const Quaternion& q);

	// Scalar power
	Vec3<T> operator^(const T scalar) const;
	Vec3<T>& operator^=(const T scalar);

	// Equality, checks if all elements are equal
	bool operator==(const Vec3<T>& rhs) const;
	bool operator!=(const Vec3<T>& rhs) const;

	constexpr T DotProduct(const Vec3<T>& rhs) const;
	constexpr static T DotProduct(const Vec3<T>& lhs, const Vec3<T>& rhs);

	constexpr Vec3<T> CrossProduct(const Vec3<T>& rhs) const;
	constexpr static Vec3<T> CrossProduct(const Vec3<T>& lhs, const Vec3<T>& rhs);

	Vec3<T> GetNormalized() const;
	Vec3<T>& Normalize();
	T GetMagnitude() const;
	T GetMagnitudeSquared() const;
};

template <typename T>
Vec3<T> Vec3<T>::operator-() const {
	return Vec3<T>(-x, -y, -z);
}

template <typename T>
Vec3<T> Vec3<T>::operator+(const T scalar) const {
	return Vec3<T>(x + scalar, y + scalar, z + scalar);
}

template <typename T>
Vec3<T>& Vec3<T>::operator+=(const T scalar) {
	x += scalar;
	y += scalar;
	z += scalar;

	return *this;
}

template <typename T>
Vec3<T> Vec3<T>::operator-(const T scalar) const {
	return Vec3<T>(x - scalar, y - scalar, z - scalar);
}

template <typename T>
Vec3<T>& Vec3<T>::operator-=(const T scalar) {
	x -= scalar;
	y -= scalar;
	z -= scalar;

	return *this;
}

template <typename T>
Vec3<T> Vec3<T>::operator+(const Vec3<T>& rhs) const {
	return Vec3<T>(x + rhs.x, y + rhs.y, z + rhs.z);
}

template <typename T>
Vec3<T>& Vec3<T>::operator+=(const Vec3<T>& rhs) {
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;

	return *this;
}

template <typename T>
Vec3<T> Vec3<T>::operator-(const Vec3<T>& rhs) const {
	return Vec3<T>(x - rhs.x, y - rhs.y, z - rhs.z);
}

template <typename T>
Vec3<T>& Vec3<T>::operator-=(const Vec3<T>& rhs) {
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;

	return *this;
}

template <typename T>
Vec3<T> Vec3<T>::operator*(const T scalar) const {
	return Vec3<T>(x * scalar, y * scalar, z * scalar);
}

template <typename T>
Vec3<T>& Vec3<T>::operator*=(const T scalar) {
	x *= scalar;
	y *= scalar;
	z *= scalar;

	return *this;
}

template <typename T>
Vec3<T> Vec3<T>::operator/(const T scalar) const {
	if (scalar == T(0)) {
		throw std::runtime_error("Vec3::operator/ - Attempted to divide by zero");
	}

	return Vec3<T>(x / scalar, y / scalar, z / scalar);
}

template <typename T>
Vec3<T>& Vec3<T>::operator/=(const T scalar) {
	if (scalar == T(0)) {
		throw std::runtime_error("Vec3::operator/= - Attempted to divide by zero");
	}

	x /= scalar;
	y /= scalar;
	z /= scalar;

	return *this;
}

template <typename T>
Vec3<T> Vec3<T>::operator*(const Vec3<T>& rhs) const {
	return Vec3<T>(x * rhs.x, y * rhs.y, z * rhs.z);
}

template <typename T>
Vec3<T>& Vec3<T>::operator*=(const Vec3<T>& rhs) {
	x *= rhs.x;
	y *= rhs.y;
	z *= rhs.z;

	return *this;
}

template <typename T>
Vec3<T> Vec3<T>::operator/(const Vec3<T>& rhs) const {
	if (rhs.x == 0 || rhs.y == 0 || rhs.z == 0) {
		throw std::runtime_error("Vec3::operator/ - Attempted to divide by zero with a vector component");
	}

	return Vec3<T>(x / rhs.x, y / rhs.y, z / rhs.z);
}

template <typename T>
Vec3<T>& Vec3<T>::operator/=(const Vec3<T>& rhs) {
	if (rhs.x == 0 || rhs.y == 0 || rhs.z == 0) {
		throw std::runtime_error("Vec3::operator/= - Attempted to divide by zero with a vector component");
	}

	x /= rhs.x;
	y /= rhs.y;
	z /= rhs.z;

	return *this;
}

template <typename T>
Vec3<T> Vec3<T>::operator^(const T scalar) const {
	return Vec3<T>(std::pow(x, scalar), std::pow(y, scalar), std::pow(z, scalar));
}

template <typename T>
Vec3<T>& Vec3<T>::operator^=(const T scalar) {
	x = std::pow(x, scalar);
	y = std::pow(y, scalar);
	z = std::pow(z, scalar);

	return *this;
}

template <typename T>
bool Vec3<T>::operator==(const Vec3<T>& rhs) const {
	return x == rhs.x && y == rhs.y && z == rhs.z;
}

template <typename T>
bool Vec3<T>::operator!=(const Vec3<T>& rhs) const {
	return !(*this == rhs);
}

template <typename T>
constexpr T Vec3<T>::DotProduct(const Vec3<T>& rhs) const {
	return x * rhs.x + y * rhs.y + z * rhs.z;
}

template <typename T>
inline constexpr T Vec3<T>::DotProduct(const Vec3<T>& lhs, const Vec3<T>& rhs) {
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

template <typename T>
Vec3<T> Vec3<T>::GetNormalized() const {
	T mag = this->GetMagnitude();

	if (mag == 0) {
		throw std::runtime_error("Vec3::GetNormalized - Attempted to divide by a zero magnitude");
	}

	return Vec3<T>(x / mag, y / mag, z / mag);
}

template <typename T>
Vec3<T>& Vec3<T>::Normalize() {
	T mag = this->GetMagnitude();

	if (mag == 0) {
		throw std::runtime_error("Vec3::Normalize - Attempted to divide by a zero magnitude");
	}

	x /= mag;
	y /= mag;
	z /= mag;

	return *this;
}

template <typename T>
T Vec3<T>::GetMagnitude() const {
	return std::hypot(x, y, z);
}

template <typename T>
T Vec3<T>::GetMagnitudeSquared() const {
	return x * x + y * y + z * z;
}