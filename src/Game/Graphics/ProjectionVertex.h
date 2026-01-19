#pragma once
#include "Math/Vec4.h"
#include "Graphics/Colour.h"

class ProjectionVertex {
public:
	ProjectionVertex(Vec4<float> clipPos, Colour colour) : m_clipPosition(clipPos), m_colour(colour) {};


	const Vec4<float>& GetClipPosition() const { return m_clipPosition; }
	const Colour& GetColour() const { return m_colour; }

	void SetColour(int r, int g, int b, int a = 255) { m_colour = {r / 255.f, g / 255.f, b / 255.f, a / 255.f}; }

private:
	Vec4<float> m_clipPosition;
	Colour m_colour;
};