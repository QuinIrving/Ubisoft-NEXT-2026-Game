#pragma once
#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Math/Vec4.h"

class ViewVertex {
public:
	ViewVertex(Vec3<float> worldPos, Vec3<float> viewPos, Vec3<float> worldNormal, Vec3<float> viewNormal, Vec3<float> viewTangent, Vec2<float> uv, Vec4<float> colour, uint32_t meshIndex, uint32_t uniqueIndex)
		: m_worldPosition(worldPos), m_viewPosition(viewPos), m_worldNormal(worldNormal), m_viewNormal(viewNormal), m_viewTangent(viewTangent), m_UV(uv), m_colour(colour), m_meshIndex(meshIndex), m_uniqueIndex(uniqueIndex) {};

	//Vec4<float> operator*(const Mat4<float>& rhs) const { return m_position * rhs; }

	const Vec3<float>& GetWorldPosition() const { return m_worldPosition; }
	const Vec3<float>& GetViewPosition() const { return m_viewPosition; }
	const Vec3<float>& GetWorldNormal() const { return m_worldNormal; }
	const Vec3<float>& GetViewNormal() const { return m_viewNormal; }
	const Vec3<float>& GetViewTangent() const { return m_viewTangent; }
	const Vec2<float>& GetUV() const { return m_UV; }
	const Vec4<float>& GetColour() const { return m_colour; }
	uint32_t GetMeshIndex() const { return m_meshIndex; }
	uint32_t GetUniqueIndex() const { return m_uniqueIndex; }

	void SetColour(int r, int g, int b, int a = 255) { m_colour = {r / 255.f, g / 255.f, b / 255.f, a / 255.f}; }

	void SetMeshIndex(uint32_t meshIndex) { m_meshIndex = meshIndex; }
	void SetUniqueIndex(uint32_t index) { m_uniqueIndex = index; }

private:
	Vec3<float> m_worldPosition{};
	Vec3<float> m_viewPosition{};

	Vec3<float> m_worldNormal;
	Vec3<float> m_viewNormal;

	Vec3<float> m_viewTangent;

	Vec2<float> m_UV;
	Vec4<float> m_colour;


	uint32_t m_meshIndex; // for tessellation
	uint32_t m_uniqueIndex;
};