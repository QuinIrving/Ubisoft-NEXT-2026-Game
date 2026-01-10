#pragma once
#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Graphics/Colour.h"

class ScreenSpaceVertex {
public:
	ScreenSpaceVertex(Vec2<float> screenPos, float z, float w, Colour colour) : m_screenPosition(screenPos), z(z), w(w), m_colour({ colour.r, colour.g, colour.b }) {};

	//Vec4<float> operator*(const Mat4<float>& rhs) const { return m_position * rhs; }

	const Vec2<float>& GetScreenPosition() const { return m_screenPosition; }
	float GetDepth() const { return z; }
	float GetW() const { return w; }
	const Vec3<float>& GetColour() const { return m_colour; }

	void SetColour(int r, int g, int b) { m_colour = {r / 255.f, g / 255.f, b / 255.f}; }

private:
	Vec2<float> m_screenPosition;
	float z;
	float w;
	//float invW; // ? may not need

	Vec3<float> m_colour;
};