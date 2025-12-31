#pragma once
#include "Math/Vec3.h"

class Triangle {
public:
	static Vec3<float> ComputeFaceNormal(const Vec3<float>& A, const Vec3<float>& B, const Vec3<float>& C) {
		const Vec3<float> u = C - A;
		const Vec3<float> v = B - A;

		return Vec3<float>::CrossProduct(u, v).GetNormalized();
	}
};