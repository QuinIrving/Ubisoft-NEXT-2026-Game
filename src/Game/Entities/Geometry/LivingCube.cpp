#include "LivingCube.h"
#include <AppSettings.h>

/*
struct Wall {
	float bottomLeft;
	float topRight;

	std::vector<Vertex> baseVerts;
	std::vector<Vertex> triangulatedVerts;
};
*/

LivingCube::LivingCube(float width, float height, float depth) : width(width), height(height), depth(depth) {
	auto addWall = [&](Vec3<float> bl, Vec3<float> tr, Vec3<float> normal) {
		Wall w;
		w.bottomLeft = bl; // Assuming your struct uses Vec3 or custom types
		w.topRight = tr;
		w.normal = normal;

		w.col = Colour(FRAND_RANGE(0, 255) / 255.f, FRAND_RANGE(0, 255) / 255.f, FRAND_RANGE(0, 255) / 255.f, 1.f);

		// Determine the two intermediate corners based on which axis is constant
		Vec3<float> v0, v1, v2, v3;

		if (bl.y == tr.y) { // Horizontal Face (Bottom/Top - XZ plane)
			v0 = bl; // {x, y, z}
			v1 = { tr.x, bl.y, bl.z };
			v2 = tr;
			v3 = { bl.x, bl.y, tr.z };
		}
		else if (bl.z == tr.z) { // Vertical Face (Left/Right - XY plane)
			v0 = bl;
			v1 = { tr.x, bl.y, bl.z };
			v2 = tr;
			v3 = { bl.x, tr.y, bl.z };
		}
		else { // Vertical Face (Front/Back - YZ plane)
			v0 = bl;
			v1 = { bl.x, tr.y, bl.z };
			v2 = tr;
			v3 = { bl.x, bl.y, tr.z };
		}

		// Triangle 1
		w.baseVerts.push_back(Vertex(v0, { 1,1,1,1 }, normal));
		w.baseVerts.push_back(Vertex(v1, { 1,1,1,1 }, normal));
		w.baseVerts.push_back(Vertex(v2, { 1,1,1,1 }, normal));

		// Triangle 2
		w.baseVerts.push_back(Vertex(v0, { 1,1,1,1 }, normal));
		w.baseVerts.push_back(Vertex(v2, { 1,1,1,1 }, normal));
		w.baseVerts.push_back(Vertex(v3, { 1,1,1,1 }, normal));

		walls.push_back(w);
	};

	// 1. Bottom face (Normal: Down)
	addWall({ 0, 0, 0 }, { width, 0, depth }, { 0, -1, 0 });

	// 2. Top face (Normal: Up)
	addWall({ 0, height, 0 }, { width, height, depth }, { 0, 1, 0 });

	// 3. Left face (Normal: Back - based on your coords)
	addWall({ 0, 0, 0 }, { width, height, 0 }, { 0, 0, -1 });

	// 4. Right face (Normal: Front)
	addWall({ 0, 0, depth }, { width, height, depth }, { 0, 0, 1 });

	// 5. Front face (Normal: Left)
	addWall({ 0, 0, 0 }, { 0, height, depth }, { -1, 0, 0 });

	// 6. Back face (Normal: Right)
	addWall({ width, 0, 0 }, { width, height, depth }, { 1, 0, 0 });

	//walls.push_back();
	/*Wall w;

	// bottom face.
	w.bottomLeft = { 0, 0, 0 };
	w.topRight = { width, 0, depth };

	walls.push_back(w);

	// top face.
	w.bottomLeft = { 0, height, 0 };
	w.topRight = { width, height, depth };

	walls.push_back(w);

	// Left wall:
	w.bottomLeft = { 0, 0, 0 };
	w.topRight = { width, height, 0 };

	walls.push_back(w);

	// Right wall
	w.bottomLeft = { 0, 0, depth };
	w.topRight = { width, height, depth };

	walls.push_back(w);

	// Front wall
	w.bottomLeft = { 0, 0, 0 };
	w.topRight = { 0, height, depth };

	walls.push_back(w);

	// Back wall
	w.bottomLeft = { width, 0, 0 };
	w.topRight = { width, height, depth };

	walls.push_back(w);*/
}