#pragma once
#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Math/Vec4.h"
#include "VertexPostClip.h"

class VertexOut {
public:
	VertexOut(Vec4<float> pos) : m_position(pos) {}
	VertexOut(float x, float y, float z, float w) : m_position({ x, y, z, w }) {}
	VertexOut(Vec4<float> pos, Vec3<float> worldPos, Vec3<float> viewPos, Vec4<float>colour, Vec3<float> normal, Vec2<float> uv, Vec3<float> tangent, Vec3<float> bitangent)
		: m_position(pos), m_worldPosition(worldPos), m_viewPosition(viewPos), m_colour(colour), m_normal(normal), m_UV(uv), m_tangent(tangent), m_bitangent(bitangent) {
	};

	Vec4<float> operator*(const Mat4<float>& rhs) const { return m_position * rhs; }

	const Vec4<float>& GetPosition() const { return m_position; }
	const Vec3<float>& GetWorldPosition() const { return m_worldPosition; }
	const Vec3<float>& GetViewPosition() const { return m_viewPosition; }
	const Vec4<float>& GetColour() const { return m_colour; }
	const Vec3<float>& GetNormal() const { return m_normal; }
	const Vec2<float>& GetUV() const { return m_UV; }
	const Vec3<float>& GetTangent() const { return m_tangent; }
	const Vec3<float>& GetBitangent() const { return m_bitangent; }

	void SetColour(int r, int g, int b, int a = 255) { m_colour = {r / 255.f, g / 255.f, b / 255.f, a / 255.f}; }

	VertexPostClip PerspectiveDivide() const {
		float invW = 1 / m_position.w;
		Vec3<float> newPos = { m_position.x / m_position.w, m_position.y / m_position.w, m_position.z / m_position.w };
		
		return VertexPostClip(newPos, invW, m_worldPosition, m_viewPosition, m_colour, m_normal, m_UV, m_tangent, m_bitangent);
	}

private:
	Vec4<float> m_position;
	Vec3<float> m_worldPosition{}; // vert in's multiplied only by M, for normal calculations during triangle assembly lol.
	Vec3<float> m_viewPosition{}; // vertex in's multiplied only by MV, for normal calculations during triangle assembly.
	Vec4<float> m_colour;
	Vec3<float> m_normal;
	Vec2<float> m_UV;// = { 0, 0 };
	Vec3<float> m_tangent;// = { 0, 0, 0 };
	Vec3<float> m_bitangent = { 0, 0, 0 };
};