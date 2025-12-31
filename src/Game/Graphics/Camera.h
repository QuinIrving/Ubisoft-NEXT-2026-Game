#pragma once
#include "Math/MathConstants.h"
#include "Math/Quaternion.h"
#include "Math/Vec2.h"
#include "Math/Vec4.h"
#include "Math/Mat4.h"
#include "Math/MatVecQuat_ops.h"

class Camera {
public:
	static constexpr float MAX_PITCH = 89.f * DEGREE_TO_RADIANS;
	Camera() = default;

	void RotateXY(float addPitch, float addYaw);
	void Translate(float x, float y, float z);
	void Translate(const Vec3<float>& v);

	Quaternion GetRotation() const { return m_rotation; }
	Vec4<float> GetPosition() const { return m_position; }
	Vec2<float> GetPitchYaw() const { return { m_pitch, m_yaw }; }
	float GetPitch() const { return m_pitch; } // Along X-AXIS -> UP/DOWN camera movement
	float GetYaw() const { return m_yaw; } // Along Y-AXIS -> RIGHT/LEFT camera movement

	Mat4<float> GetViewMatrix() const;

	float GetMouseSensitivty() const { return m_senstivity; }
	void SetMouseSensitivty(float sense) { m_senstivity = sense; }

private:
	Quaternion m_rotation{};
	Vec4<float> m_position{};

	float m_pitch;
	float m_yaw;
	float m_senstivity = 100;
};