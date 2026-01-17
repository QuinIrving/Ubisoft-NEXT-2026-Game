#include "Player.h"

Mat4<float> Player::GetViewMatrix() {
	c.SetPosition(m_position + Vec3<float>(0, -m_eyeHeight, 0));
	return c.GetViewMatrix();
}

// We calculate our wishdir via these two and the input direction given by our keyboard input.
Vec3<float> Player::GetForwardDir() {
	Quaternion qYaw = Quaternion(Vec3<float>(0, 1, 0), c.GetYaw());
	return Vec4<float>(0, 0, -1, 0) * qYaw;
}

Vec3<float> Player::GetRightDir() {
	Quaternion qYaw = Quaternion(Vec3<float>(0, 1, 0), c.GetYaw());
	return Vec4<float>(1, 0, 0, 0) * qYaw;
}