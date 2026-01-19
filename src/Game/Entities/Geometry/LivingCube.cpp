#include "LivingCube.h"
#include <AppSettings.h>

LivingCube::LivingCube(float width, float height, float depth) : width(width), height(height), depth(depth) {
	auto addWall = [&](Vec3<float> bl, Vec3<float> tr, Vec3<float> normal) {
		Wall w;
		w.bottomLeft = bl;
		w.topRight = tr;
		w.normal = normal;

		w.col = Colour(FRAND_RANGE(0, 255) / 255.f, FRAND_RANGE(0, 255) / 255.f, FRAND_RANGE(0, 255) / 255.f, 1.f);

		Vec3<float> v0, v1, v2, v3;

		if (bl.y == tr.y) { // horizontal face
			v0 = bl; // {x, y, z}
			v1 = { tr.x, bl.y, bl.z };
			v2 = tr;
			v3 = { bl.x, bl.y, tr.z };
		}
		else if (bl.z == tr.z) { // Left/Right face
			v0 = bl;
			v1 = { tr.x, bl.y, bl.z };
			v2 = tr;
			v3 = { bl.x, tr.y, bl.z };
		}
		else { // Front/back face
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

	
	addWall({ 0, 0, 0 }, { width, 0, depth }, { 0, -1, 0 }); // Bottom face
	addWall({ 0, height, 0 }, { width, height, depth }, { 0, 1, 0 }); // Top face
	addWall({ 0, 0, 0 }, { width, height, 0 }, { 0, 0, -1 }); // Left face
	addWall({ 0, 0, depth }, { width, height, depth }, { 0, 0, 1 }); // Right face
	addWall({ 0, 0, 0 }, { 0, height, depth }, { -1, 0, 0 }); // Front face
	addWall({ width, 0, 0 }, { width, height, depth }, { 1, 0, 0 });// Back face
}