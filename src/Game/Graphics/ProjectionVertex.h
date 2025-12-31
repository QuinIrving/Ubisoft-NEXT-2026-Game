#pragma once
#include "Math/Vec4.h"

class ProjectionVertex {
public:
	ProjectionVertex(Vec4<float> clipPos, Vec4<float> colour) : m_clipPosition(clipPos), m_colour(colour) {};

	//Vec4<float> operator*(const Mat4<float>& rhs) const { return m_position * rhs; }

	const Vec4<float>& GetClipPosition() const { return m_clipPosition; }
	const Vec4<float>& GetColour() const { return m_colour; }

	void SetColour(int r, int g, int b, int a = 255) { m_colour = {r / 255.f, g / 255.f, b / 255.f, a / 255.f}; }

private:
	Vec4<float> m_clipPosition;
	Vec4<float> m_colour;
};