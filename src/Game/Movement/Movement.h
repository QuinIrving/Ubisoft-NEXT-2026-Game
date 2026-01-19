#pragma once
#include "Math/Vec3.h"
class Player;

// This is for all of the states that the player can be in
enum MovementState {
	GROUND,
	AIR,
	GRAPPLEHOOK,
};


namespace MovementSystem {
	const float JUMP_IMPULSE = 48.f;

	void HandlePlayerMovement(Player& p, Vec3<float> inputDir, float delta);
}