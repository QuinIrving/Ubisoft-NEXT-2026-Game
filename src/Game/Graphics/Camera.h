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

	void SetPosition(Vec3<float> pos) { m_position = Vec4<float>(pos); }
	void SetRotation(float pitch, float yaw) { m_pitch = pitch; m_yaw = yaw; }

	Quaternion GetRotation() const { return m_rotation; }
	Vec4<float> GetPosition() const { return m_position; }
	Vec2<float> GetPitchYaw() const { return { m_pitch, m_yaw }; }
	float GetPitch() const { return m_pitch; } // Along X-AXIS -> UP/DOWN camera movement
	float GetYaw() const { return m_yaw; } // Along Y-AXIS -> RIGHT/LEFT camera movement

	Mat4<float> GetViewMatrix() const;

	float GetMouseSensitivty() const { return m_mouseSens; }
	void SetMouseSensitivty(float sense) { m_mouseSens = sense; }
	float GetControllerSensitivty() const { return m_controllerSens; }
	void SetControllerSensitivty(float sense) { m_controllerSens = sense; }

private:
	Quaternion m_rotation{};
	Vec4<float> m_position{};

	float m_pitch;
	float m_yaw;
	float m_mouseSens = 100;
	float m_controllerSens = 250;
};