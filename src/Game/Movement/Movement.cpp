#include "Movement.h"
#include "Entities/Player/Player.h"
#include "Math/MathConstants.h"
#include <algorithm>
#include <cmath>

namespace {
	// Our generic settings;
	const float MAX_VELOCITY = 40.f;
	const float GRAVITY = 25.0f; // 20 was good
	const float MAX_OFF_PLATFORM_ALLOWED_TIME = 0.15f; // in seconds
	const float STOP_SPEED = 2.4f; // Ratio from CS: (75/250) * our ground speed of 8 instead.
}

namespace GroundSettings {
	const float ACCELERATION = 5.5f;
	const float SPEED = 8.f;
	const float FRICTION = 5.2f;
}

namespace AirSettings {
	//const float ACCELERATION = 20.f;
	const float ACCELERATION = 50.f;
	const float SPEED = GroundSettings::SPEED;
	const float MAX_SURF_SPEED = 45.f;
	const float DRAG = 0.1f;

}

namespace Movement {
	void Move(Player& p, Vec3<float> wishDir, float accel, float maxSpeed, float friction, float delta) {
		
		if (friction > 0) {
			float currSpeed = p.GetVelocity().GetMagnitude();
			if (currSpeed != 0) {
				float control = (currSpeed < STOP_SPEED) ? STOP_SPEED : currSpeed;
				//float drop = currSpeed * friction * delta;
				float drop = control * friction * delta;
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
		Movement::HandleGroundMovement(p, wishDir, delta);
		break;
	case MovementState::AIR:
		Movement::HandleAirMovement(p, wishDir, delta);
		break;
	case MovementState::GRAPPLEHOOK:
		break;
	}

	
	// Clamp our horizontal speed to our max air speed to make BHOP and SURF still feel nice, without making it go with crazy gains
	if (p.GetMoveState() == MovementState::AIR) {
		Vec3<float> vel = p.GetVelocity();
		float horzMag = Vec2<float>(vel.x, vel.z).GetMagnitude();

		if (horzMag > AirSettings::MAX_SURF_SPEED) {
			float scale = AirSettings::MAX_SURF_SPEED / horzMag;
			vel.x *= scale;
			vel.z *= scale;
			p.SetVelocity(vel);
		}
	}


	p.UpdateOffGroundTimer(delta);
	float yVel = -GRAVITY * delta;

	/*if (p.GetMoveState() == MovementState::GROUND) {
		yVel = 0.f;
	}*/
	
	p.UpdateVelocity(Vec3<float>(0, yVel, 0));

	

	// collision should continue to update this back to 0, and set position, and movestate to ground

	//update / transition right after(may need to swap these if it feels off)
	// Then pass to the correct handler, as we only really update if we're already back from collision to be grounded, or we are over the time to jump.
	if (p.GetMoveState() == MovementState::GROUND && p.GetTimeOffGround() > MAX_OFF_PLATFORM_ALLOWED_TIME) {
		p.TransitionMoveState(MovementState::AIR);
		p.ResetOffGroundTimer();
	}

	// This is where our collision detection should be, could hand it off, to properly do the real position update.

	//p.UpdatePosition(p.GetVelocity() * delta);
}

