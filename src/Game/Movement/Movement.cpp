#include "Movement.h"
#include "Entities/Player/Player.h"
#include "Math/MathConstants.h"
#include <algorithm>
#include <cmath>

namespace {
	// Our generic settings;
	const float MAX_VELOCITY = 30.f;
	const float GRAVITY = 20.0f;
	const float JUMP_IMPULSE = 8.f;
	const float MAX_OFF_PLATFORM_ALLOWED_TIME = 0.15f; // in seconds
}

namespace GroundSettings {
	const float ACCELERATION = 7.f;
	const float SPEED = 8.f;
	const float FRICTION = 8.f;
}

namespace AirSettings {
	const float ACCELERATION = 100.f;
	const float SPEED = 1.f;
	//const float SPEED = 0.1f;
	//const float DRAG = 0.1f;
	const float DRAG = 0.f;

}

namespace {
	void Move(Player& p, Vec3<float> wishDir, float accel, float maxSpeed, float friction, float delta) {
		
		if (friction > 0) {
			float currSpeed = p.GetVelocity().GetMagnitude();
			if (currSpeed != 0) {
				float drop = currSpeed * friction * delta;
				float newSpeed = std::max<float>(0.f, currSpeed - drop);
				p.SetVelocity(p.GetVelocity() * (newSpeed / currSpeed));
			}
		}
		
		// Project curr vel onto wishDir
		float currSpeedInWish = p.GetVelocity().DotProduct(wishDir);

		// how much speed can we add without exceeding maxSpeed
		//float addSpeed = MAX_VELOCITY - currSpeedInWish;
		float addSpeed = maxSpeed - currSpeedInWish;

		if (addSpeed <= 0.f) {
			return;
		}

		float accelSpeed = accel * delta * maxSpeed;

		if (accelSpeed > addSpeed) {
			accelSpeed = addSpeed;
		}

		p.SetVelocity(p.GetVelocity() + (wishDir * accelSpeed));
	}
	
	// Our helper functions
	void HandleGroundMovement(Player& p, Vec3<float> wishDir, float delta) {
		Move(p, wishDir, GroundSettings::ACCELERATION, GroundSettings::SPEED, GroundSettings::FRICTION, delta);
	}

	void HandleAirMovement(Player& p, Vec3<float> wishDir, float delta) {
		//Vec3<float> currentVel = p.GetVelocity();
		//currentVel.y = 0; // Only care about horizontal speed for strafing

		//float speed = currentVel.GetMagnitude();

		//if (speed == 0.f) {
		//	return;
		//}

		//// 1. Calculate the angle between where we are going and where we want to go
		//// Dot product of normalized vectors gives the Cosine of the angle
		//float dot = currentVel.GetNormalized().DotProduct(wishDir);
		//float angle = acosf(std::clamp(dot, -1.0f, 1.0f));

		//// 2. Sample our "Curve"
		//// We want a peak multiplier when the angle is around 90 degrees (PI/2)
		//float strafeMultiplier = 1.0f;

		//// Example "Curve" Logic: 
		//// If angle is near 90 deg (PI/2), boost the acceleration significantly
		//float targetAngle = PI / 2.0f;
		//float angleDiff = fabsf(angle - targetAngle);

		//if (angle < PI / 2.0f) {
		//	// Boost speed when "Slightly off-center" (The Air Strafe Zone)
		//	// This is where you gain momentum
		//	strafeMultiplier = 1.0f + (expf(-angleDiff * 2.0f) * 5.0f);
		//}
		//else {
		//	// Penalize speed if trying to turn too sharply (> 90 degrees)
		//	strafeMultiplier = 0.2f;
		//}

		//// 3. Apply the modified acceleration
		//float accel = AirSettings::ACCELERATION * strafeMultiplier;

		//float airAccel = 100.f;
		//float airCap = 1.f;

		//Move(p, wishDir, airAccel, airCap, AirSettings::DRAG, delta);
		Move(p, wishDir, AirSettings::ACCELERATION, AirSettings::SPEED, AirSettings::DRAG, delta);
	}
}

void MovementSystem::HandlePlayerMovement(Player& p, Vec3<float> inputDir, float delta) {
	Vec3<float> wishDir = (p.GetForwardDir() * inputDir.z) + (p.GetRightDir() * inputDir.x);
	wishDir.y = 0;
	
	if (wishDir.x != 0.f || wishDir.z != 0.f) {
		wishDir.Normalize();
	}

	switch (p.GetMoveState()) {
	case MovementState::GROUND:
		HandleGroundMovement(p, wishDir, delta);
		break;
	case MovementState::AIR:
		HandleAirMovement(p, wishDir, delta);
		p.UpdateVelocity(Vec3<float>(0, GRAVITY * delta, 0));
		break;
	case MovementState::GRAPPLEHOOK:
		break;
	}

	p.UpdateOffGroundTimer(delta);

	// collision should continue to update this back to 0, and set position, and movestate to ground

	//update / transition right after(may need to swap these if it feels off)
	// Then pass to the correct handler, as we only really update if we're already back from collision to be grounded, or we are over the time to jump.
	if (p.GetMoveState() == MovementState::GROUND && p.GetTimeOffGround() > MAX_OFF_PLATFORM_ALLOWED_TIME) {
		p.TransitionMoveState(MovementState::AIR);
		p.ResetOffGroundTimer();
	}

	p.UpdatePosition(p.GetVelocity() * delta);
}

