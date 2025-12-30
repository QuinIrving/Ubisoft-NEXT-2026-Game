#pragma once
#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Math/Vec4.h"
#include "Math/MatVecQuat_ops.h"

class Vertex {
public:
	Vertex() = default;
	Vertex(Vec3<float> pos) : m_position(pos) {}
	Vertex(float x, float y, float z) : m_position({ x, y, z }) {}
	constexpr Vertex(Vec3<float> pos, Vec4<float> col = { 1, 1, 1, 1 }, Vec3<float> norm = { 0, 0, 1 }, Vec2<float> uv = {}, Vec3<float> tan = {}, float tanW = 0.f) :
		m_position(pos), m_colour(col), m_normal(norm), m_UV(uv), m_tangent(tan), m_tangentW(tanW) {}

	Vec4<float> operator*(const Mat4<float>& rhs) const { return m_position * rhs; }

	const Vec3<float>& GetPosition() const { return m_position; }
	const Vec4<float>& GetColour() const { return m_colour; }
	const Vec3<float>& GetNormal() const { return m_normal; }
	const Vec2<float>& GetUV() const { return m_UV; }
	const Vec3<float>& GetTangent() const { return m_tangent; }
	const float GetTangentW() const { return m_tangentW; }

	void SetNormal(Vec3<float> norm) { m_normal = norm; }
	void SetColour(int r, int g, int b, int a = 255) { m_colour = { r / 255.f, g / 255.f, b / 255.f, a / 255.f }; }
	void SetTangent(Vec3<float> t) { m_tangent = t; }
	void SetTangentW(float tw) { m_tangentW = tw; }

private:
	Vec3<float> m_position = { 0, 0, 0 };
	Vec4<float> m_colour = { 1, 1, 1, 1 };
	Vec3<float> m_normal = { 0, 0, 1 };
	Vec2<float> m_UV = { 0, 0 };
	Vec3<float> m_tangent = { 0, 0, 0 };
	float m_tangentW = 0.f; // should be -1 or 1 if tangent is defined.
};