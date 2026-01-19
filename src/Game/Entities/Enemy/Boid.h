#pragma once
#include "Math/Vec3.h"
#include <Math/Quaternion.h>
#include <vector>
#include <Graphics/Vertex.h>
#include <Graphics/Colour.h>


// We'll represent it with ~4 faces. Tetrahedron.
class Boid {
public:
	// we want them able to know nearby neighbours, and move in that direction. Our rotation handles all of the movement, we want it kind of pointy to easily see the vertex
	// it uses as it's "front", and for nice easy OBB.
	Boid(Vec3<float> startPos={0, 0, 0});

	void Rotate(float x, float y, float z);
	void Scale(float x, float y, float z);
	void Translate(float x, float y, float z);
	void SetPosition(float x, float y, float z) { m_position = { x, y, z }; }

	Colour GetColour() const { return col; }

	Mat4<float> GetModelMatrix() const;
	std::vector<Vertex>& GetVertices() { return m_vertices; }
	Mat4<float> GetRotationMatrix() { return m_delta.GetRotationMatrix(); }
	Vec3<float> GetTranslation() { return m_position; }
	Vec3<float> GetScale() { return m_scale; }
	Vec3<float> GetLocalHalfExtent() { return Vec3<float>(); }

	void Update(std::vector<Boid>& allBoids, float delta);
	Vec3<float> GetForward() const { return Vec3<float>(0, 1, 0) * m_delta; }
	void RotateTowards(Vec3<float> targetDir, float delta);

private:
	Vec3<float> m_position = { 0, 0, 0 };
	Vec3<float> m_scale = { 1.f, 1.f, 1.f };
	Quaternion m_delta;

	Colour col = { 1.f, 1.f, 1.f, 1.f };

	float m_visualAngle = 120; // The degrees which it can see
	float m_minSpeed = 0.5f;
	float m_maxSpeed = 2.f;
	float m_turnFactor = 1.f;
	float m_radius = 2.5f; // Angle with this should help for obstacle avoidance
	Vec3<float> m_goalWorldPos; // This would be our player, unless maybe something forces it to want something else.
	// Our mesh for this is simply a std::vector of pre-initialized Vertices. Might need to refactor for something cleaner for the pipeline.
	std::vector<Vertex> m_vertices;
};
