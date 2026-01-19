#pragma once
#include "Math/Vec3.h"

struct CapsuleCollider {
	float height = 1.8f;
	float radius = 0.35f;
	float width = 0.7f;
	Vec3<float> direction = Vec3<float>(0, -1, 0); // vertical axis
};