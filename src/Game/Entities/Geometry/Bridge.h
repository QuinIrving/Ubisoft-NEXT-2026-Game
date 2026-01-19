#pragma once
#include "Math/Vec3.h"
#include "Graphics/Vertex.h"
#include <vector>
#include "Graphics/Colour.h"


struct OBB;

enum class BridgeClass {
	TriangularPrism, // Had issues with sliding and colliding and stuff, so bypass it.
	Box,
};

enum class BridgeState {
	Expanding,
	FullyExtended,
	Contracting,
	Inactive
};

// Redundant with my other objects.
struct KinematicWall {
	Vec3<float> bottomLeft;
	Vec3<float> topRight;
	Vec3<float> normal;

	Colour col;

	std::vector<Vertex> staticVerts; // Actually are all verts lol

	bool isCapFace = false;
};

class Bridge {
public:
	// can either be a triangular prism or a rectangle, but either way we don't mind, as both are just vertices;
	// just need to bear in mind my constraints for each shape.

	// This way of constructing makes it relatively trivial to create both the triangle and box types.
	Bridge(Vec3<float> bottomLeft, Vec3<float> bottomRight, Vec3<float> topCenter, BridgeClass type, Vec3<float> translateAxis, float secBetweenTransitions, float growthSpeed = 5.f);
	void Update(float delta, float maxLength);

	BridgeState state = BridgeState::Inactive;

	void Translate(Vec3<float> translate) { translation += translate; }
	std::vector<KinematicWall>& GetWalls() { return walls; }
	Vec3<float> GetTranslate() { return translation; }
	Vec3<float> GetTranslateAxis() { return translateAxis; }
	std::vector<Vertex> GetVertices(); // need to do a smart combine with our kinematic vertices and static vertices.
	std::vector<OBB> GetCollisionBoxes(Vec3<float> playerHalfSize);
	float GetGrowthSpeed() { return growthSpeed; }

	BridgeClass type;

private:
	Vec3<float> translation = { 0, 0, 0 }; // THis is how we will expand our kinematic verts (and tessellated if we get to that point)
	Vec3<float> scale = { 1, 1, 1 };
	std::vector<KinematicWall> walls;
	float growthSpeed;
	float timeUntilTransition;
	Vec3<float> translateAxis;
	float timeElapsedStatic = 0.f;
};