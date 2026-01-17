#pragma once
#include <Graphics/Camera.h>
#include "Movement/Movement.h"

class Player {
public:
	Player() = default;

	Vec3<float> GetPosition() const { return m_position; }
	Vec3<float> GetVelocity() const { return m_velocity; }
	float GetTimeOffGround() const { return m_offGroundTimer; }
	MovementState GetMoveState() const { return m_moveState; }

	void UpdatePosition(Vec3<float> translation) { m_position += translation; }
	void UpdateOffGroundTimer(float delta) { m_offGroundTimer += delta; }
	void UpdateVelocity(Vec3<float> velocity) { m_velocity += velocity; }
	
	void TransitionMoveState(MovementState newState) { m_moveState = newState; }

	// This will make you rotate around x-axis (up/down), and rotate around y-axis (left/right).
	void RotateXY(float addXRot, float addYRot) { c.RotateXY(addXRot, addYRot); }
	Mat4<float> GetViewMatrix();
	
	// Need a way to get the looking direction/orientation to do my proper computations in movement updates.
	Vec3<float> GetForwardDir();
	Vec3<float> GetRightDir();

private:
	Vec3<float> m_position;
	Vec3<float> m_velocity;

	float m_eyeHeight = 1.8f;
	MovementState m_moveState = MovementState::GROUND;
	float m_offGroundTimer = 0.0f; // Used to allow a brief period after going off the ground, to allow players to still jump

	Camera c;

};