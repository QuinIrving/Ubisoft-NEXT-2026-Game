#include "Boid.h"
#include <Graphics/Triangle.h>
#include <typeinfo>
#include <algorithm>

Boid::Boid(Vec3<float> startPos) : m_position(startPos) {
	// we want a tetrahedron.
	
	// start with the "base" verts
	Vec3<float> p0 = { -0.5f, -1.f, -0.5f };
	Vec3<float> p1 = { 0.f, -1.f, 0.5f };
	Vec3<float> p2 = { 0.5f, -1.f, -0.5f };
	
	// center point:
	Vec3<float> center = { 0, 1.f, 0 };

	Vec4<float> col = { 1.f, 1.f, 1.f, 1.f };
	Vec3<float> baseNormal = Triangle::ComputeFaceNormal(p0, p1, p2); // This will mean flat shading instead of Gouraud for boids, but that should be fine with it's harsh edges.

	// baseFace:
	Vertex f0V0 = Vertex(p0, col, baseNormal);
	Vertex f0V1 = Vertex(p1, col, baseNormal);
	Vertex f0V2 = Vertex(p2, col, baseNormal);

	// Side 1 face:
	Vec3<float> side1Normal = Triangle::ComputeFaceNormal(p0, center, p1);
	Vertex f1V0 = Vertex(p0, col, side1Normal);
	Vertex f1V1 = Vertex(center, col, side1Normal);
	Vertex f1V2 = Vertex(p1, col, side1Normal);

	// Side 2 face:
	Vec3<float> side2Normal = Triangle::ComputeFaceNormal(p1, center, p2);
	Vertex f2V0 = Vertex(p1, col, side2Normal);
	Vertex f2V1 = Vertex(center, col, side2Normal);
	Vertex f2V2 = Vertex(p2, col, side2Normal);

	// Side 3 face:
	Vec3<float> side3Normal = Triangle::ComputeFaceNormal(p2, center, p0);
	Vertex f3V0 = Vertex(p2, col, side3Normal);
	Vertex f3V1 = Vertex(center, col, side3Normal);
	Vertex f3V2 = Vertex(p0, col, side3Normal);
	

	m_vertices.insert(m_vertices.end(), { f0V0, f0V1, f0V2, f1V0, f1V1, f1V2, f2V0, f2V1, f2V2, f3V0, f3V1, f3V2 });
	// 
	//Vertex v0 = Vertex({0, 0, 0}, {0, 0, 0}, );
}

void Boid::Rotate(float x, float y, float z) {
	Quaternion deltaRotation = Quaternion(x, y, z);

	m_delta = deltaRotation * m_delta;
	m_delta.Normalize();
}

void Boid::Scale(float x, float y, float z) {
	m_scale.x *= x;
	m_scale.y *= y;
	m_scale.z *= z;
}

void Boid::Translate(float x, float y, float z) {
	m_position.x += x;
	m_position.y += y;
	m_position.z += z;
}

Mat4<float> Boid::GetModelMatrix() const {
	return Mat4<float>::Scale(m_scale) * m_delta.GetRotationMatrix() * Mat4<float>::Translate(m_position);
}

void Boid::Update(std::vector<Boid>& allBoids, float delta) {
	Vec3<float> separation(0, 0, 0);
	Vec3<float> alignment(0, 0, 0);
	Vec3<float> cohesion(0, 0, 0);
	int neighbourCount = 0;

	for (Boid& b : allBoids) {
		if (&b == this) {
			continue;
		}

		Vec3<float> diffToOther = b.m_position - m_position;
		float distSquare = diffToOther.GetMagnitudeSquared();

		if (distSquare < m_radius * m_radius) {
			if (distSquare > EPSILON) {
				separation -= diffToOther / distSquare;
			}
			
			alignment += b.GetForward();
			cohesion += b.m_position;
			neighbourCount++;
		}
	}

	if (neighbourCount > 0) {
		if (alignment.GetMagnitudeSquared() > EPSILON) {
			alignment = (alignment / static_cast<float>(neighbourCount)).GetNormalized();
		}

		if (cohesion.GetMagnitudeSquared() > EPSILON) {
			cohesion = ((cohesion / static_cast<float>(neighbourCount)) - m_position).GetNormalized();
		}

		if (separation.GetMagnitudeSquared() > EPSILON) {
			separation = separation.GetNormalized();
		}
	}

	Vec3<float> steer = separation * 1.5f + alignment * 1.0f + cohesion * 1.0f;

	/*
	// add goal influence if desired
	if ((m_goalWorldPos - m_position).GetMagnitudeSquared() > 0.1f) {
		steer += (m_goalWorldPos - m_position).GetNormalized() * 0.5f;
	}*/

	if (steer.GetMagnitude() > m_turnFactor) {
		steer = steer.GetNormalized() * m_turnFactor;
	}

	// Rotate +Y forward toward steer
	Vec3<float> currentForward = GetForward();
	if (steer.GetMagnitude() > 0.001f) {
		Quaternion targetRot = Quaternion::FromTo(currentForward, steer);
		m_delta = Quaternion::Slerp(m_delta, targetRot * m_delta, m_turnFactor * delta);
	}

	// Move along +Y
	float speed = std::clamp(steer.GetMagnitude(), m_minSpeed, m_maxSpeed);
	m_position += GetForward() * speed * delta;
}

void Boid::RotateTowards(Vec3<float> targetDir, float delta) {
	if (targetDir.GetMagnitudeSquared() < 0.001f) {
		return;
	}

	targetDir.Normalize();
	Vec3<float> forward = GetForward();
	Quaternion targetRot = Quaternion::FromTo(forward, targetDir);
	m_delta = Quaternion::Slerp(m_delta, targetRot * m_delta, m_turnFactor * delta);
}