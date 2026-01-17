#include "Movement.h"
#include "Entities/Player/Player.h"

namespace {
	// Our generic settings;
	const float MAX_VELOCITY = 10.f;
	const float GRAVITY = 20.0f;
	const float JUMP_IMPULSE = 8.f;
	const float MAX_OFF_PLATFORM_ALLOWED_TIME = 17.f; // in miliseconds
}

namespace GroundSettings {
	const float ACCELERATION = 7.f;
	const float SPEED = 8.f;
	const float FRICTION = 8.f;
}

namespace AirSettings {
	const float ACCELERATION = 0.5f;
	const float SPEED = 16.f;
	const float DRAG = 0.1f;

}

namespace {
	void Move(Player& p, float accel, float speed, float friction, float delta) {

	}
	
	// Our helper functions
	void HandleGroundMovement(Player& p, float delta) {
		Move(p, GroundSettings::ACCELERATION, GroundSettings::SPEED, GroundSettings::FRICTION, delta);
	}

	void HandleAirMovement(Player& p, float delta) {
		Move(p, AirSettings::ACCELERATION, AirSettings::SPEED, AirSettings::DRAG, delta);
	}
}

/*
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
}*/



void MovementSystem::HandlePlayerMovement(Player& p, float delta) {
	switch (p.GetMoveState()) {
	case MovementState::GROUND:
		HandleGroundMovement(p, delta);
		break;
	case MovementState::AIR:
		HandleAirMovement(p, delta);
		break;
	case MovementState::GRAPPLEHOOK:
		break;
	}

	p.UpdateOffGroundTimer(delta);

	//update / transition right after(may need to swap these if it feels off)
	// Then pass to the correct handler, as we only really update if we're already back from collision to be grounded, or we are over the time to jump.
	if (p.GetMoveState() == MovementState::GROUND && p.GetTimeOffGround() > MAX_OFF_PLATFORM_ALLOWED_TIME) {
		p.TransitionMoveState(MovementState::AIR);
	}
}

