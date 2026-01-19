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

private:
	Vec3<float> m_ballPos; // should accelerate towards the player.
	Vec3<float> m_velocity;
	Vec3<float> m_acceleration;

	std::vector<Boid> m_boids;
	float m_maxSpeed = 200.f;
};
