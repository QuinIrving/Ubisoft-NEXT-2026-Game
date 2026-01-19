#include "Camera.h"
#include <algorithm>

// Currently is an FPS camera, so clamping our pitch (up/down). Takes in degrees
void Camera::RotateXY(float addPitch, float addYaw) {
	addPitch *= DEGREE_TO_RADIANS;
	addYaw *= DEGREE_TO_RADIANS;

	m_pitch += addPitch;
	m_pitch = std::clamp(m_pitch, -MAX_PITCH, MAX_PITCH);

	m_yaw += addYaw;
	m_yaw = fmodf(m_yaw, PI * 2);

	Quaternion qYaw = Quaternion(Vec3<float>(0, 1, 0), m_yaw);
	Quaternion qPitch = Quaternion(Vec3<float>(1, 0, 0), m_pitch);

	m_rotation = qYaw * qPitch;
}


void Camera::Translate(float x, float y, float z) {
	Vec3<float> forward = Vec4<float>(0, 0, -1, 0) * m_rotation; 
	Vec3<float> right = Vec4<float>(1, 0, 0, 0) * m_rotation;
	//Vec3<float> up = Vec4<float>(0, 1, 0, 0) * m_rotation;

	// To maintain FPS feel, we will ignore the y-axis
	forward.y = 0;
	forward.Normalize();

	right.y = 0;
	right.Normalize();

	m_position += forward * z;
	m_position += right * x;
	m_position += Vec3<float>(0, 1, 0) * y;
}

void Camera::Translate(const Vec3<float>& v)
{
	this->Translate(v.x, v.y, v.z);
}

Mat4<float> Camera::GetViewMatrix() const {
	Vec3<float> right = Vec4<float>(1, 0, 0, 0) * m_rotation;
	Vec3<float> up = Vec4<float>(0, -1, 0, 0) * m_rotation;
	Vec3<float> forward = Vec4<float>(0, 0, -1, 0) * m_rotation;

	float rPos = right.DotProduct(m_position);
	float uPos = up.DotProduct(m_position);
	float fPos = forward.DotProduct(m_position);

	// Row by Row
	auto row1 = Vec4<float>(right.x, up.x, -forward.x, 0);
	auto row2 = Vec4<float>(right.y, up.y, -forward.y, 0);
	auto row3 = Vec4<float>(right.z, up.z, -forward.z, 0);
	auto row4 = Vec4<float>(-rPos,	-uPos,	     fPos, 1);

	return Mat4<float>({row1, row2, row3, row4});
}