#pragma once
#include <vector>
#include "Math/Vec3.h"
#include "Graphics/Vertex.h"
#include <Graphics/Colour.h>

struct Wall {
	Vec3<float> bottomLeft;
	Vec3<float> topRight;
	Vec3<float> normal;

	Colour col;

	std::vector<Vertex> baseVerts;
	std::vector<Vertex> triangulatedVerts;
};

// Has 6 faces that contain the player, and the Bottom kills.
// There also will be moving "bridges" that are embedded in the walls.
class LivingCube {
public:
	LivingCube(float width=1.f, float height=1.f, float depth=1.f);

	std::vector<Wall>& GetWalls() { return walls; }
	float GetWidth() { return width; }
	float GetHeight() { return height; }
	float GetDepth() { return depth; }

private:
	std::vector<Wall> walls;

	float width;
	float height;
	float depth;
};