#pragma once
#include "Player/Player.h"
#include "Geometry/LivingCube.h"
#include <vector>
#include <Objects/Quad.h>
#include "Geometry/Bridge.h"
#include "Enemy/Boid.h"
#include "Enemy/Swarm.h"

// ECS would have been better, but no time, so using OOP, and this world class of different types of entities (as we don't have too many base types)
struct World {
	Player player;

	LivingCube cube;
	std::vector<Swarm> boidSwarm; // Creature is a flock of boids that follows, in the shape of a snake or something.
	std::vector<Quad> quads;
	std::vector<Bridge> bridges; // The platforms that the living cube produces, and can move. Composed of 5 (or 6) binded quads
	std::vector<Boid> boids;
	// bullets that can collide

};