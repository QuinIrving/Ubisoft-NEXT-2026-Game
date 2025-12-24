#include "Quaternion.h"

// we distinguish our quaternion as x,y,z,w, but our w acts like the standard (even though it's at the bottom)
Quaternion::Quaternion(const Vec3<float>& axis, float angle) { // axis angle
	Vec3<float> axisNormalized = axis.GetNormalized();
	float s = sinf(angle * 0.5f);

	m_delta = Vec4<float>();
	m_delta.x = axisNormalized.x * s;
	m_delta.y = axisNormalized.y * s;
	m_delta.z = axisNormalized.z * s;
	m_delta.w = cosf(angle * 0.5f);
}
Quaternion::Quaternion(float rotX, float rotY, float rotZ) { // euler angles
	// q = qx * qy * qz.
	// optimizing:
	float halfX = rotX * 0.5f;
	float halfY = rotY * 0.5f;
	float halfZ = rotZ * 0.5f;

	float cosX = cosf(halfX);
	float cosY = cosf(halfY);
	float cosZ = cosf(halfZ);

	float sinX = sinf(halfX);
	float sinY = sinf(halfY);
	float sinZ = sinf(halfZ);

	float x = (sinX * cosY * cosZ) - (cosX * sinY * sinZ);
	float y = (cosX * sinY * cosZ) + (sinX * cosY * sinZ);
	float z = (cosX * cosY * sinZ) - (sinX * sinY * cosZ);
	float w = (cosX * cosY * cosZ) + (sinX * sinY * sinZ);

	m_delta = Vec4<float>(x, y, z, w);
}

Quaternion::Quaternion(const Mat4<float>& rotMat) { // rot mat4
	// bypass for now.
}

Quaternion& Quaternion::operator=(const Quaternion& other) {
	if (this != &other) {
		m_delta = other.m_delta;
	}

	return *this;
}

Quaternion& Quaternion::operator=(Quaternion&& other) noexcept {
	if (this != &other) {
		m_delta = other.m_delta;
		other.m_delta = Vec4<float>();
	}
	return *this;
}

Quaternion Quaternion::operator~() const {
	return Quaternion(-m_delta.x, -m_delta.y, -m_delta.z, m_delta.w);
}

// Quaternion Conjugation in place.
Quaternion& Quaternion::Conjugate() {
	m_delta.x = -m_delta.x;
	m_delta.y = -m_delta.y;
	m_delta.z = -m_delta.z;

	return *this;
}


// Quaternion multiply (Hamilton Product)
Quaternion Quaternion::operator*(const Quaternion& rhs) const { // rotation composition
	Vec4<float> result;
	result.x = (m_delta.w * rhs.m_delta.x) + (rhs.m_delta.w * m_delta.x) + (m_delta.y * rhs.m_delta.z) - (m_delta.z * rhs.m_delta.y);
	result.y = (m_delta.w * rhs.m_delta.y) + (rhs.m_delta.w * m_delta.y) + (m_delta.z * rhs.m_delta.x) - (m_delta.x * rhs.m_delta.z);
	result.z = (m_delta.w * rhs.m_delta.z) + (rhs.m_delta.w * m_delta.z) + (m_delta.x * rhs.m_delta.y) - (m_delta.y * rhs.m_delta.x);
	result.w = (m_delta.w * rhs.m_delta.w) - (m_delta.x * rhs.m_delta.x) - (m_delta.y * rhs.m_delta.y) - (m_delta.z * rhs.m_delta.z);

	return Quaternion(result);
}

Quaternion& Quaternion::operator*=(const Quaternion& rhs) {
	Vec4<float> result;
	result.x = (m_delta.w * rhs.m_delta.x) + (rhs.m_delta.w * m_delta.x) + (m_delta.y * rhs.m_delta.z) - (m_delta.z * rhs.m_delta.y);
	result.y = (m_delta.w * rhs.m_delta.y) + (rhs.m_delta.w * m_delta.y) + (m_delta.z * rhs.m_delta.x) - (m_delta.x * rhs.m_delta.z);
	result.z = (m_delta.w * rhs.m_delta.z) + (rhs.m_delta.w * m_delta.z) + (m_delta.x * rhs.m_delta.y) - (m_delta.y * rhs.m_delta.x);
	result.w = (m_delta.w * rhs.m_delta.w) - (m_delta.x * rhs.m_delta.x) - (m_delta.y * rhs.m_delta.y) - (m_delta.z * rhs.m_delta.z);

	m_delta = result;
	return *this;
}

Mat4<float> Quaternion::GetRotationMatrix() const {
	Mat4<float> m = Mat4<float>::GetIdentity();

	m[0][0] = 2 * (m_delta.w * m_delta.w + m_delta.x * m_delta.x) - 1;
	m[0][1] = 2 * (m_delta.x * m_delta.y - m_delta.w * m_delta.z);
	m[0][2] = 2 * (m_delta.x * m_delta.z + m_delta.w * m_delta.y);

	m[1][0] = 2 * (m_delta.x * m_delta.y + m_delta.w * m_delta.z);
	m[1][1] = 2 * (m_delta.w * m_delta.w + m_delta.y * m_delta.y) - 1;
	m[1][2] = 2 * (m_delta.y * m_delta.z - m_delta.w * m_delta.x);

	m[2][0] = 2 * (m_delta.x * m_delta.z - m_delta.w * m_delta.y);
	m[2][1] = 2 * (m_delta.y * m_delta.z + m_delta.w * m_delta.x);
	m[2][2] = 2 * (m_delta.w * m_delta.w + m_delta.z * m_delta.z) - 1;

	/*
	float x = m_delta.x, y = m_delta.y, z = m_delta.z, w = m_delta.w;
	float xx = x * x, yy = y * y, zz = z * z;
	float xy = x * y, xz = x * z, yz = y * z;
	float wx = w * x, wy = w * y, wz = w * z;

	Mat4<float> m = Mat4<float>::GetIdentity();

	m[0][0] = 1.0f - 2.0f * (yy + zz);
	m[0][1] = 2.0f * (xy + wz);
	m[0][2] = 2.0f * (xz - wy);

	m[1][0] = 2.0f * (xy - wz);
	m[1][1] = 1.0f - 2.0f * (xx + zz);
	m[1][2] = 2.0f * (yz + wx);

	m[2][0] = 2.0f * (xz + wy);
	m[2][1] = 2.0f * (yz - wx);
	m[2][2] = 1.0f - 2.0f * (xx + yy);
	*/
	return m;
}

Quaternion& Quaternion::Normalize() {
	m_delta = m_delta.GetNormalized();

	return *this;
}

void Quaternion::Print() const {
	m_delta.Print();
}