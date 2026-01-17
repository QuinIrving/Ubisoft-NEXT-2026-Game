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
	void HandlePlayerMovement(Player& p, Vec3<float> inputDir, float delta);
}

//class MovementHandler {
//public:
//
//private:
//};