#pragma once
#include <vector>
#include "Boid.h"

class Swarm {
public:
	Swarm(unsigned int numBoids = 1) {
		for (int i = 0; i < numBoids; ++i) {
			m_boids.push_back(Boid());
		}
	}

	void Update(Vec3<float> playerPos, float delta) {
		// do our own update to our acceleration (new based on player dir), acceleration could potentially be uniform, instead changing our velocity based on a fixed acceleration and the dir.
		// update our velocity based on time with new accel
		// update our ball pos based on this as well.
		Vec3<float> toPlayer = playerPos - m_ballPos;
		float dist = toPlayer.GetMagnitude();

		if (dist > 0.01f) {
			Vec3<float> desiredVel = toPlayer.GetNormalized() * m_maxSpeed;
			Vec3<float> steer = desiredVel - m_velocity;

			float maxAccel = 100.f;
			if (steer.GetMagnitudeSquared() > maxAccel * maxAccel) {
				steer = steer.GetNormalized() * maxAccel;
			}

			m_acceleration = steer;
		}
		else {
			m_acceleration = { 0, 0, 0 };
		}
		//m_acceleration += (playerPos - m_ballPos);
		float damping = 0.8f;
		m_velocity = m_velocity * damping + m_acceleration * delta;

		if (m_velocity.GetMagnitudeSquared() > m_maxSpeed * m_maxSpeed) {
			m_velocity = m_velocity.GetNormalized() * m_maxSpeed;
		}

		m_ballPos += m_velocity * delta;

		for (int i = 0; i < m_boids.size(); ++i) {
			m_boids[i].SwarmUpdate(m_ballPos, m_boids, delta);
		}
	}

	std::vector<Boid>& GetBoids() { return m_boids; }

	/*
	void Render(RenderPipeline& p, const Mat4<float>& viewMatrix) {
		for (Boid& b : m_boids) {
			p.Render(b.GetVertices(), b.GetModelMatrix(), viewMatrix, b.GetColour());
		}
	}*/

private:
	Vec3<float> m_ballPos; // should accelerate towards the player.
	Vec3<float> m_velocity;
	Vec3<float> m_acceleration;

	std::vector<Boid> m_boids;
	float m_maxSpeed = 200.f;
};

/*
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
	Vec3<float> GetPosition() { return m_position; }
	Vec3<float> GetScale() { return m_scale; }
	Vec3<float> GetLocalHalfExtent() { return Vec3<float>(); }

	void Update(std::vector<Boid>& allBoids, Vec3<float> playerPos, float delta);
	Vec3<float> GetForward() const { return Vec3<float>(0, 1, 0) * m_delta; }
	void RotateTowards(Vec3<float> targetDir, float delta);

	Vec3<float> SteerTowards(Vec3<float> desiredDir);

private:
	Vec3<float> m_position = { 0, 0, 0 };
	Vec3<float> m_scale = { 1.f, 1.f, 1.f };
	Quaternion m_delta;

	Colour col = { 1.f, 1.f, 1.f, 1.f };

	Vec3<float> m_velocity;
	float m_visualAngle = 270.f;
	float m_visualLimit = std::cosf(m_visualAngle / 2.f * DEGREE_TO_RADIANS); // The degrees which it can see
	float m_minSpeed = 10.f;
	float m_maxSpeed = 30.f;
	float m_turnFactor = 5.f;
	float m_perceptionRadius = 12.f; // Angle with this should help for obstacle avoidance
	float m_separationRadius = 12.f;
	// Our mesh for this is simply a std::vector of pre-initialized Vertices. Might need to refactor for something cleaner for the pipeline.
	std::vector<Vertex> m_vertices;
	Vec3<float> m_acceleration = { 0.f, 0.f, 0.f };
};*/
