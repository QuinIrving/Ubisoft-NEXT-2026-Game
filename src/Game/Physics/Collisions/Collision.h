#pragma once
#include "Entities/World.h"

// Will need the list of objects we can collide with (ECS would be amazing, but idk if I have time at this point)
// as well as the position of the current object, it's updated velocity, and delta time. That way we can find everything along our path of our line segment of where we will head
// We will assume our character is a AABB instead of a capsule for this specific implementation (unless I have extra time later, which I doubt)
// Off the top of my head, Oct-tree for partioning the space of our level, could severely speed up finding objects, as we do O(log n) top find our space(s) of our line segment, and the objects
// within that we can loop through.

// BSP tree like quake is a nice idea, but I may have to keep with a sweeping check, and currently iterate over all objects, and change my data structure as necessary.
// ^ don't think i could, as it's typically pre-computed and I have moving parts? So a quad/oct tree space partitioned is probably the best approach.


// Doing a "Collide- and slide" seems like a good approach, and until we get to a small enough velocity or max iterations, we can search, then collide/slide,
// pass back our new pos, new velocity, and do this over and over again to get the next object to collide with, and handle it.

enum EntityType {
	PLAYER,
	QUAD,
	LIVING_CUBE, // perhaps per face? not sure
	BRIDGE,
	BOID,
	BULLET,
	CREATURE, // idk should be the boid but whatever
	DisplacedPoint,
	SPIKE, // it is a type of displacepoint so idk. More for me to remember when handling the other collisions
};

struct Collide {
	bool hasCollision;
	EntityType type;
	int index;
	float time;
	Vec3<float> collisionNormal;
	Vec3<float> kinematicSurfaceVel = { 0.f, 0.f, 0.f };
};

namespace Collision {
	void ResolvePlayerCollision(World& w, float delta);
	Collide DetectPlayerCollision(World& w, float delta); // Currently handles static only, for dynamic, will need to worry about each face? or can we just do one and move on?
	void HandlePlayerCollisionResolution(World& w, Collide& c, float& delta);
}