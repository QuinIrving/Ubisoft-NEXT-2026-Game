#pragma once
#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Math/Vec4.h"

class VertexOut {
public:
	VertexOut(Vec4<float> pos) : m_position(pos) {}
	VertexOut(float x, float y, float z, float w) : m_position({ x, y, z, w }) {}
	VertexOut(Vec4<float> pos, Vec3<float> worldPos, Vec3<float> viewPos, Vec3<float> normal, Vec2<float> uv, Vec3<float> tangent, Vec3<float> bitangent)
		: m_position(pos), m_worldPosition(worldPos), m_viewPosition(viewPos), m_normal(normal), m_UV(uv), m_tangent(tangent), m_bitangent(bitangent) {
	};

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