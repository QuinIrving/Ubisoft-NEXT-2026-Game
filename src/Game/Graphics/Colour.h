#pragma once
#include "Math/Vec3.h"
#include "Math/Vec4.h"

struct Colour {
	Colour() : r(1.f), g(1.f), b(1.f), a(1.f) {}
	Colour(float r, float g, float b, float a = 1.f) : r(r), g(g), b(b), a(a) {}
	Colour(Vec3<float> v, float a) : r(v.x), g(v.y), b(v.z), a(a) {};
	Colour(Vec4<float> v) : r(v.x), g(v.y), b(v.z), a(v.w) {};

	Colour operator*(float scale) const {
		return Colour(r * scale, g * scale, b * scale, a);
	}

	Colour& operator*=(float scale) {
		r *= scale;
		g *= scale;
		b *= scale;

		return *this;
	}

	Colour operator+(const Colour& rhs) const {
		return Colour(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a);
	}

	Vec3<float> GetVectorizedRGB() const {
		return Vec3<float>(r, g, b);
	}

	Vec4<float> GetVectorizedRGBA() const {
		return Vec4<float>(r, g, b, a);
	}

	float r;
	float g;
	float b;
	float a;
};