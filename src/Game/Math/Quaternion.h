#pragma once
#include "Mat4.h"
#include "Vec4.h"
#include "Vec3.h"

class Quaternion {
public:
	Quaternion() : m_delta(Vec4<float>(0.f, 0.f, 0.f, 1.f)) {};
	Quaternion(float x, float y, float z, float w) : m_delta(Vec4<float>(x, y, z, w)) {};
	Quaternion(float real, const Vec3<float>& v) : m_delta(Vec4<float>(v.x, v.y, v.z, real)) {};
	Quaternion(const Vec4<float>& delta) : m_delta(delta) {};
	Quaternion(const Vec3<float>& axis, float angle); // axis angle
	Quaternion(float rotX, float rotY, float rotZ); // euler angles
	Quaternion(const Mat4<float>& rotMat); // rotation matrix

	Quaternion(const Quaternion& other) : m_delta(other.m_delta) {};
	Quaternion& operator=(const Quaternion& other);
	Quaternion(Quaternion&& other) noexcept : m_delta(other.m_delta) { other.m_delta = Vec4<float>(); }
	Quaternion& operator=(Quaternion&& other) noexcept;

	// Quaternion Conjugation
	Quaternion operator~() const;
	Quaternion& Conjugate();

	Vec4<float> GetDelta() { return m_delta; }
	Mat4<float> GetRotationMatrix() const;

	// Quaternion multiply (Hamilton Product)
	Quaternion operator*(const Quaternion& rhs) const; // rotation composition
	Quaternion& operator*=(const Quaternion& rhs);

	Quaternion& Normalize();

	void Print() const;

private:
	Vec4<float> m_delta; // x, y, z are the vector components.
};