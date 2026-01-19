#include "Quaternion.h"
#include "MathConstants.h"

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
Quaternion::Quaternion(float rotX, float rotY, float rotZ) { // euler angles -> ZYX convention -> Takes in Degrees
	rotX *= DEGREE_TO_RADIANS;
	rotY *= DEGREE_TO_RADIANS;
	rotZ *= DEGREE_TO_RADIANS;

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

/*
Quaternion::Quaternion(const Mat4<float>& rotMat) { // rot mat4
	// bypass for now.
	throw new std::runtime_error("Quaternion constructor given a mat4 rotation matrix is not yet implemented.\n");
}*/

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
	Quaternion normalized = Quaternion(*this).Normalize();
	Vec4<float> delta = normalized.GetDelta();

	Mat4<float> m = Mat4<float>::GetIdentity();

	m[0][0] = 2 * (delta.w * delta.w + delta.x * delta.x) - 1;
	m[0][1] = 2 * (delta.x * delta.y - delta.w * delta.z);
	m[0][2] = 2 * (delta.x * delta.z + delta.w * delta.y);

	m[1][0] = 2 * (delta.x * delta.y + delta.w * delta.z);
	m[1][1] = 2 * (delta.w * delta.w + delta.y * delta.y) - 1;
	m[1][2] = 2 * (delta.y * delta.z - delta.w * delta.x);

	m[2][0] = 2 * (delta.x * delta.z - delta.w * delta.y);
	m[2][1] = 2 * (delta.y * delta.z + delta.w * delta.x);
	m[2][2] = 2 * (delta.w * delta.w + delta.z * delta.z) - 1;

	return m;
}

Quaternion& Quaternion::Normalize() {
	m_delta = m_delta.GetNormalized();

	return *this;
}

void Quaternion::Print() const {
	m_delta.Print();
}

Quaternion Quaternion::FromTo(const Vec3<float>& from, const Vec3<float>& to) {
	Vec3<float> f = from.GetNormalized();
	Vec3<float> t = to.GetNormalized();

	// Finds how much in the direction of "to", that "from" already is.
	float fromProjTo = f.DotProduct(t);
	Vec3<float> rotationAxis;

	// Check if we're already pointing in direction of to.
	if (fromProjTo >= 1.f - EPSILON) {
		return Quaternion(0, 0, 0, 1);
	}

	// Check if we are facing opposite direction
	if (fromProjTo <= -1.f + EPSILON) {
		
		rotationAxis = Vec3<float>(1, 0, 0).CrossProduct(f);

		// If we are essentially pointing in the same direction as the axis, lets use a different axis
		if (rotationAxis.GetMagnitudeSquared() < EPSILON) {
			rotationAxis = Vec3<float>(0, 1, 0).CrossProduct(f);
		}

		rotationAxis.Normalize();
		return Quaternion(rotationAxis, PI); // rotate 180 degs
	}
	
	rotationAxis = t.CrossProduct(f);
	float s = std::sqrtf((1 + fromProjTo) * 2);
	float invs = 1 / s;
	rotationAxis *= invs;
	return Quaternion(rotationAxis.x, rotationAxis.y, rotationAxis.z, s * 0.5f).Normalize();
}

Quaternion Quaternion::Slerp(Quaternion q1, Quaternion q2, float time) {
	
	Vec4<float> delta1 = q1.m_delta;
	Vec4<float> delta2 = q2.m_delta;
	float q1InQ2 = (delta1).DotProduct(delta2);

	if (q1InQ2 < 0.f) {
		delta2 = -delta2;
		q1InQ2 = -q1InQ2;
	}

	if (q1InQ2 > 1.f - EPSILON) {
		return Quaternion((delta1 + (delta2 - delta1) * time).GetNormalized());
	}

	float theta = std::acosf(q1InQ2);
	float sinv = 1.f / std::sin(theta);
	float weight1 = std::sinf((1 - time) * theta) * sinv;
	float weight2 = std::sinf(time * theta) * sinv;

	return Quaternion((delta1 * weight1 + delta2 * weight2).GetNormalized());

	// similar to our physics we are focused on time between 0-1
	/*if (time <= 0) {
		return q1;
	}

	if (time >= 1) {
		return q2;
	}

	// How close q1 is to q2 in rotation direction
	float q1ProjQ2 = q1.m_delta.DotProduct(q2.m_delta);
	Quaternion q2b = q2;

	if (q1ProjQ2 < 0.f) {
		q2b.m_delta = -q2b.m_delta; // take shorter path
		q1ProjQ2 = -q1ProjQ2;
	}

	if (q1ProjQ2 > 1.f - EPSILON) {
		// linear fall back
		Vec4<float> result = q1.m_delta + (q2b.m_delta - q1.m_delta) * time;
		return Quaternion(result).Normalize();
	}

	float theta = acos(q1ProjQ2); // angle between quaternions
	float intervalTheta = theta * time;

	Quaternion q3 = Quaternion(q2b.m_delta - q1.m_delta * q1ProjQ2).Normalize();
	return Quaternion(q1.m_delta * cos(theta) + q3.m_delta * sin(theta)).Normalize();*/
}