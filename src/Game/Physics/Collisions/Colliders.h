#pragma once
#include "Math/Vec3.h"
#include "Math/Vec4.h"

struct CapsuleCollider {
	float height = 1.8f;
	float radius = 0.35f;
	float width = 0.7f;
	Vec3<float> direction = Vec3<float>(0, -1, 0); // vertical axis
};

struct OBB {
	Vec3<float> center; // model matrix translation
	Vec3<float> halfEdges; // quad width/height/depth / 2
	Mat4<float> rotation; // need to extract the upper 3x3 corner, and divide it by the scale factor the object already has.

	OBB(Vec3<float> c, Vec3<float> he, Mat4<float> r) : center(c), halfEdges(he), rotation(r) {};
};