#pragma once
#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Math/Vec4.h"

// OLD! CAN REMOVE

class VertexPostClip {
public:
	VertexPostClip(Vec3<float> position, float invW, Vec3<float> worldPosition, Vec3<float> viewPosition, Vec4<float> colour, Vec3<float> normal, Vec2<float> UV, Vec3<float> tangent, Vec3<float> bitangent)
		: m_position(position), m_invW(invW), m_worldPosition(worldPosition), m_viewPosition(viewPosition), m_colour(colour), m_normal(normal), m_UV(UV), m_tangent(tangent), m_bitangent(bitangent) {
	};

	const Vec4<float>& GetPosition() const { return m_position; }
	float GetInvW() const { return m_invW; }
	const Vec3<float>& GetWorldPosition() const { return m_worldPosition; }
	const Vec3<float>& GetViewPosition() const { return m_viewPosition; }
	const Vec4<float>& GetColour() const { return m_colour; }
	const Vec3<float>& GetNormal() const { return m_normal; }
	const Vec2<float>& GetUV() const { return m_UV; }
	const Vec3<float>& GetTangent() const { return m_tangent; }
	const Vec3<float>& GetBitangent() const { return m_bitangent; }

private:
	Vec4<float> m_position;
	float m_invW;
	Vec3<float> m_worldPosition;//{}; // vert in's multiplied only by M, for normal calculations during triangle assembly lol.
	Vec3<float> m_viewPosition;//{}; // vertex in's multiplied only by MV, for normal calculations during triangle assembly.
	Vec4<float> m_colour;
	Vec3<float> m_normal;
	Vec2<float> m_UV;// = { 0, 0 };
	Vec3<float> m_tangent;// = { 0, 0, 0 };
	Vec3<float> m_bitangent;// = { 0, 0, 0 };
};