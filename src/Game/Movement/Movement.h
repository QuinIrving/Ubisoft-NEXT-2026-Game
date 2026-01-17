#pragma once
class Player;

// This is for all of the states that the player can be in
enum MovementState {
	GROUND,
	AIR,
	GRAPPLEHOOK,
};


namespace MovementSystem {
	void HandlePlayerMovement(Player& p, float delta);
}

//class MovementHandler {
//public:
//
//private:
//};