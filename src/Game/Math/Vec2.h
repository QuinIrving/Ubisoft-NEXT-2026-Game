#pragma once
#include <stdexcept>
#include <cmath>

template <typename T>
class Vec2 {
public:
	T x{};
	T y{};

public:
	constexpr Vec2() = default;
	constexpr Vec2(T x, T y) : x(x), y(y) {};

	Vec2<T> operator-() const; // negation

	// Vector addition/subtraction
	Vec2<T> operator+(const Vec2<T>& rhs) const;
	Vec2<T>& operator+=(const Vec2<T>& rhs);
	Vec2<T> operator-(const Vec2<T>& rhs) const;
	Vec2<T>& operator-=(const Vec2<T>& rhs);

	// Scalar multiplication
	Vec2<T> operator*(const T scalar) const;
	Vec2<T>& operator*=(const T scalar);
	Vec2<T> operator/(const T scalar) const;
	Vec2<T>& operator/=(const T scalar);

	// Scalar power
	Vec2<T> operator^(const T scalar) const;
	Vec2<T>& operator^=(const T scalar);

	// Equality, checks if all elements are equal
	bool operator==(const Vec2<T>& rhs) const;
	bool operator!=(const Vec2<T>& rhs) const;

	constexpr T DotProduct(const Vec2<T>& rhs) const;
	constexpr static T DotProduct(const Vec2<T>& lhs, const Vec2<T>& rhs);

	constexpr T CrossProduct(const Vec2<T>& rhs) const;
	constexpr static T CrossProduct(const Vec2<T>& lhs, const Vec2<T>& rhs);

	Vec2<T> GetNormalized() const;
	T GetMagnitude() const;
	T GetMagnitudeSquared() const;
};

template <typename T>
Vec2<T> Vec2<T>::operator-() const {
	return Vec2<T>(-x, -y);
}

template <typename T>
Vec2<T> Vec2<T>::operator+(const Vec2<T>& rhs) const {
	return Vec2<T>(x + rhs.x, y + rhs.y);
}

template <typename T>
Vec2<T>& Vec2<T>::operator+=(const Vec2<T>& rhs) {
	x += rhs.x;
	y += rhs.y;

	return *this;
}

template <typename T>
Vec2<T> Vec2<T>::operator-(const Vec2<T>& rhs) const {
	return Vec2<T>(x - rhs.x, y - rhs.y);
}

template <typename T>
Vec2<T>& Vec2<T>::operator-=(const Vec2<T>& rhs) {
	x -= rhs.x;
	y -= rhs.y;

	return *this;
}

template <typename T>
Vec2<T> Vec2<T>::operator*(const T scalar) const {
	return Vec2<T>(x * scalar, y * scalar);
}

template <typename T>
Vec2<T>& Vec2<T>::operator*=(const T scalar) {
	x *= scalar;
	y *= scalar;

	return *this;
}

template <typename T>
Vec2<T> Vec2<T>::operator/(const T scalar) const {
	if (scalar == T(0)) {
		throw std::runtime_error("Vec2::operator/ - Attempted to divide by zero");
	}

	return Vec2<T>(x / scalar, y / scalar);
}

template <typename T>
Vec2<T>& Vec2<T>::operator/=(const T scalar) {
	if (scalar == T(0)) {
		throw std::runtime_error("Vec2::operator/= - Attempted to divide by zero");
	}

	x /= scalar;
	y /= scalar;

	return *this;
}

template <typename T>
Vec2<T> Vec2<T>::operator^(const T scalar) const {
	return Vec2<T>(std::pow(x, scalar), std::pow(y, scalar));
}

template <typename T>
Vec2<T>& Vec2<T>::operator^=(const T scalar) {
	x = std::pow(x, scalar);
	y = std::pow(y, scalar);

	return *this;
}

template <typename T>
bool Vec2<T>::operator==(const Vec2<T>& rhs) const {
	return x == rhs.x && y == rhs.y;
}

template <typename T>
bool Vec2<T>::operator!=(const Vec2<T>& rhs) const {
	return !(*this == rhs);
}

template <typename T>
constexpr T Vec2<T>::DotProduct(const Vec2<T>& rhs) const {
	return x * rhs.x + y * rhs.y;
}

template <typename T>
inline constexpr T Vec2<T>::DotProduct(const Vec2<T>& lhs, const Vec2<T>& rhs) {
	return lhs.x * rhs.x + lhs.y * rhs.y;
}

template <typename T>
constexpr T Vec2<T>::CrossProduct(const Vec2<T>& rhs) const {
	return x * rhs.y - y * rhs.x;
}

template <typename T>
inline constexpr T Vec2<T>::CrossProduct(const Vec2<T>& lhs, const Vec2<T>& rhs) {
	return lhs.x * rhs.y - lhs.y * rhs.x;
}

template <typename T>
Vec2<T> Vec2<T>::GetNormalized() const {
	T mag = this->GetMagnitude();

	if (mag == 0) {
		throw std::runtime_error("Vec2::GetNormalized - Attempted to divide by a zero magnitude");
	}

	return Vec2<T>(x / mag, y / mag);
}

template <typename T>
T Vec2<T>::GetMagnitude() const {
	return std::hypot(x, y);
}

template <typename T>
T Vec2<T>::GetMagnitudeSquared() const {
	return x * x + y * y;
}