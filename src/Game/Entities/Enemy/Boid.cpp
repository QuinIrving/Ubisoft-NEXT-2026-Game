#include "Boid.h"
#include <Graphics/Triangle.h>
#include <typeinfo>
#include <algorithm>
#include <AppSettings.h>

namespace {
	float cohesionStrength = 8000.f;

	float separationRadius = 1.5f;
	float separationStrength = 1.5f;

	float orbitStrength = 0.2f;

	float wanderStrength = 0.1f;

	float ballRadius = 1.f;
}

void Boid::SwarmUpdate(const Vec3<float>& ballPos, std::vector<Boid>& boids, float delta) {
	// cohesion to ball,
	// separation from nearby neighbours
	// orbital/tangential force for a swirl effect
	// random wander

	Vec3<float> toCenter = ballPos - m_position;
	//Vec3<float> cohesion = toCenter.GetNormalized() * cohesionStrength; // idk about normalizing the direction, but we'll say sure.
	
	Vec3<float> cohesion = { 0, 0, 0 };
	float distToCenter = toCenter.GetMagnitude();

	float innerRadius = ballRadius * 0.3f;
	float outerRadius = ballRadius;

	if (distToCenter > innerRadius) {
		float factor = (distToCenter - innerRadius) / (outerRadius - innerRadius);
		factor = std::clamp(factor, 0.f, 1.f);
		//cohesion = toCenter.GetNormalized() * cohesionStrength * std::max<float>(distToCenter / 10.f, 1.f);;//std::min<float>(distToCenter / 10.f, 1.f);
		cohesion = toCenter.GetNormalized() * cohesionStrength * factor;
	}

	if (cohesion.GetMagnitudeSquared() > m_maxSpeed * m_maxSpeed) {
		cohesion = cohesion.GetNormalized() * (m_maxSpeed * 0.5f);
	}

	Vec3<float> separation(0, 0, 0);
	
	for (Boid& b : boids) {
		if (&b == this) {
			continue;
		}

		Vec3<float> posDiff = m_position - b.GetPosition();
		float dist = posDiff.GetMagnitude();

		if (dist < separationRadius && dist > EPSILON) {
			//separation += posDiff.GetNormalized() / dist;
			float factor = (separationRadius - dist) / separationRadius;
			//separation += posDiff / dist;
			separation += posDiff.GetNormalized() * separationStrength * factor;
		}
		else if (dist < separationRadius) {
			// we must be completely colliding.
			separation += posDiff;
		}
	}

	separation *= separationStrength;

	Vec3<float> axes[] = {
		Vec3<float>(0, 1, 0),
		Vec3<float>(1, 0, 0),
		Vec3<float>(0, 0, 1)
	};

	Vec3<float> offset = m_position - ballPos;
	Vec3<float> orbit(0, 0, 0);

	for (auto& axis : axes) {
		Vec3<float> tangent = offset.CrossProduct(axis);

		if (tangent.GetMagnitudeSquared() > EPSILON) {
			tangent.Normalize();
			orbit += tangent;
		}
	}

	// Orbit center
	
	/*Vec3<float> tangent = offset.CrossProduct(Vec3<float>(0, 1, 0)); // can mix and match multiple axes
	
	if (tangent.GetMagnitudeSquared() > EPSILON) {
		tangent.Normalize();
	}

	Vec3<float> orbit = tangent * orbitYStrength;*/
	//orbit *= orbitStrength;
	orbit = orbit.GetNormalized() * orbitStrength;

	Vec3<float> wander = Vec3<float>(FRAND_RANGE(-1.f, 1.f), FRAND_RANGE(-1.f, 1.f), FRAND_RANGE(-1.f, 1.f)).GetNormalized() * wanderStrength;

	m_acceleration = cohesion + separation + orbit + wander;
	//m_acceleration = cohesion + separation;
	//m_acceleration = cohesion;
	/*Vec3<float> desiredVel = m_velocity * m_acceleration * delta;
	float smoothing = 0.1f;
	m_velocity = m_acceleration * (1.f - smoothing) + desiredVel * smoothing;*/
	
	float damping = 0.8f;
	//m_velocity += m_acceleration * delta;
	m_velocity = m_velocity * damping + m_acceleration * delta;

	float speedSq = m_velocity.GetMagnitudeSquared();
	if (speedSq > m_maxSpeed * m_maxSpeed) m_velocity = m_velocity.GetNormalized() * m_maxSpeed;
	if (speedSq < m_minSpeed * m_minSpeed) m_velocity = m_velocity.GetNormalized() * m_minSpeed;

	// Update position
	m_position += m_velocity * delta;

	// optionally can rotate our boid mesh.
	if (m_velocity.GetMagnitudeSquared() > EPSILON) {
		RotateTowards(m_velocity, delta);
	}
}
/*
void Boid::Update(std::vector<Boid>& allBoids, Vec3<float> playerPos, float delta) {
	Vec3<float> separation(0, 0, 0);
	Vec3<float> alignment(0, 0, 0);
	Vec3<float> cohesion(0, 0, 0);
	int neighbourCount = 0;

	for (Boid& b : allBoids) {
		if (&b == this) {
			continue;
		}

		Vec3<float> diff = b.m_position - m_position;
		float dist = diff.GetMagnitude();

		if (dist < EPSILON || dist > m_perceptionRadius) {
			continue;
		}

		alignment += b.m_velocity;
		cohesion += b.m_position;


		float strength = 1.f - (dist / m_separationRadius);
		strength = std::max<float>(strength, 0.f);
		separation += (-diff) * strength;


		neighbourCount++;

	}

	Vec3<float> steer(0, 0, 0);

	// If we have others help guide our steering, lets update our velocity based on it.
	if (neighbourCount > 0) {
		float invCount = 1.f / neighbourCount;

		alignment *= invCount;
		if (alignment.GetMagnitudeSquared() > EPSILON) {
			Vec3<float> desired = alignment.GetNormalized() * m_maxSpeed;
			alignment = desired - m_velocity;
		}

		cohesion *= invCount;
		Vec3<float> toCenter = cohesion - m_position;
		if (toCenter.GetMagnitudeSquared() > EPSILON) {
			Vec3<float> desired = toCenter.GetNormalized() * m_maxSpeed;
			cohesion = desired - m_velocity;
		}

		separation *= invCount;
		if (separation.GetMagnitudeSquared() > EPSILON) {
			Vec3<float> desired = separation.GetNormalized() * m_maxSpeed;
			separation = desired - m_velocity;
		}


		m_acceleration += alignment * 1.0f;
		m_acceleration += cohesion * 0.8f;
		m_acceleration += separation * 1.6f;
	if (m_acceleration.GetMagnitudeSquared() > m_turnFactor * m_turnFactor) {
		m_acceleration = m_acceleration.GetNormalized() * m_turnFactor;
	}

	m_velocity += m_acceleration * delta;

	// Finally, limit max speed so they don't accelerate to infinity
	if (m_velocity.GetMagnitudeSquared() > m_maxSpeed * m_maxSpeed) {
		m_velocity = m_velocity.GetNormalized() * m_maxSpeed;
	}
	else if (m_velocity.GetMagnitudeSquared() < m_minSpeed * m_minSpeed) {
		m_velocity = m_velocity.GetNormalized() * m_minSpeed;
	}

	m_position += m_velocity * delta;
	if (m_velocity.GetMagnitudeSquared() > EPSILON) {
		RotateTowards(m_velocity, delta);
	}
}*/

Vec3<float> Boid::SteerTowards(Vec3<float> desiredDir) {
	Vec3<float> desiredVelocity = desiredDir.GetNormalized() * m_maxSpeed;
	Vec3<float> steer = desiredVelocity - m_velocity;
	return steer.Normalize() * m_turnFactor;
}

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

	Vec3<float> velocities = { FRAND_RANGE(m_minSpeed, m_maxSpeed), FRAND_RANGE(m_minSpeed, m_maxSpeed) , FRAND_RANGE(m_minSpeed, m_maxSpeed) };
	Vec3<float> signs = { FRAND_RANGE(-1, 1), FRAND_RANGE(-1, 1) , FRAND_RANGE(-1, 1) };

	m_velocity = Vec3<float>((signs.x < 0) ? -velocities.x : velocities.x, (signs.y < 0) ? -velocities.y : velocities.y, (signs.z < 0) ? -velocities.z : velocities.z);
	// Lets rotate to face the starting velocity.
	Vec3<float> forward = GetForward();
	Vec3<float> target = m_velocity.GetNormalized();

	//Vec3<float> cross = forward.CrossProduct(target);
	Vec3<float> cross = forward.CrossProduct(target);
	float dot = forward.DotProduct(target);
	m_delta = Quaternion(cross.x, cross.y, cross.z, 1 + dot);
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

void Boid::Update(std::vector<Boid>& allBoids, Vec3<float> playerPos, float delta) {
	Vec3<float> separation(0, 0, 0);
	Vec3<float> alignment(0, 0, 0);
	Vec3<float> cohesion(0, 0, 0);
	int neighbourCount = 0;

	for (Boid& b : allBoids) {
		if (&b == this) {
			continue;
		}

		Vec3<float> diff = b.m_position - m_position;
		float dist = diff.GetMagnitude();

		if (dist < EPSILON || dist > m_perceptionRadius) {
			continue;
		}

		/*float angleDiff = 0.f;
		if (distSquare > EPSILON) {
			angleDiff = m_velocity.GetNormalized().DotProduct(diff.GetNormalized());
		}*/
		alignment += b.m_velocity;
		cohesion += b.m_position;

		/*
		if (dist < m_separationRadius) {
			separation += (-diff).GetNormalized() * (m_separationRadius - dist);
		}*/

		float strength = 1.f - (dist / m_separationRadius);
		strength = std::max<float>(strength, 0.f);
		separation += (-diff) * strength;

		/*Vec3<float> away = (-diff).GetNormalized();
		float sepStrength = (m_separationRadius - dist) / m_separationRadius;
		separation += away * sepStrength;
		*/
		
		neighbourCount++;

		// check if we can see the neighbour.
		/*if (distSquare < m_radius * m_radius/* && angleDiff > m_visualLimit* /) {
			alignment += b.m_velocity;
			cohesion += b.m_position;

			if (distSquare > EPSILON) {
				separation += (m_position - b.m_position).GetNormalized();
			}

			neighbourCount++;
		}*/
	}

	Vec3<float> steer(0, 0, 0);

	// If we have others help guide our steering, lets update our velocity based on it.
	if (neighbourCount > 0) {
		float invCount = 1.f / neighbourCount;

		alignment *= invCount;
		if (alignment.GetMagnitudeSquared() > EPSILON) {
			Vec3<float> desired = alignment.GetNormalized() * m_maxSpeed;
			alignment = desired - m_velocity;
		}

		cohesion *= invCount;
		Vec3<float> toCenter = cohesion - m_position;
		if (toCenter.GetMagnitudeSquared() > EPSILON) {
			Vec3<float> desired = toCenter.GetNormalized() * m_maxSpeed;
			cohesion = desired - m_velocity;
		}

		separation *= invCount;
		if (separation.GetMagnitudeSquared() > EPSILON) {
			Vec3<float> desired = separation.GetNormalized() * m_maxSpeed;
			separation = desired - m_velocity;
		}


		m_acceleration += alignment * 1.0f;
		m_acceleration += cohesion * 0.8f;
		m_acceleration += separation * 1.6f;

		/*
		steer += alignment * 1.0f;
		steer += cohesion * 0.8f;
		steer += separation * 1.6f;*/

		//alignment = (alignment / neighbourCount).GetNormalized();
		//cohesion = (cohesion / neighbourCount).GetNormalized();
		// Alignment: Steer towards average velocity
		//alignment = (alignment / static_cast<float>(neighbourCount)) - m_velocity;
		/*Vec3<float> avgVel = alignment / neighbourCount;
		Vec3<float> desiredAlignment = avgVel.GetNormalized() * m_maxSpeed;

		alignment = desiredAlignment - m_velocity;

		// Cohesion: Steer towards average position
		Vec3<float> centerOfMass = cohesion / (float)neighbourCount;
		Vec3<float> desiredCohesion = (centerOfMass - m_position).GetNormalized() * m_maxSpeed;
		cohesion = desiredCohesion - m_velocity;

		separation /= neighbourCount;

		if (separation.GetMagnitudeSquared() > EPSILON) {
			Vec3<float> desiredSeparation = separation.GetNormalized() * m_maxSpeed;
			separation = desiredSeparation - m_velocity;
		}

		// Combine with weights
		steer = (alignment * 1.0f) + (cohesion * 1.0f) + (separation * 1.5f);*/
	}

	//float maxForce = 4.f;
	if (m_acceleration.GetMagnitudeSquared() > m_turnFactor * m_turnFactor) {
		m_acceleration = m_acceleration.GetNormalized() * m_turnFactor;
	}

	/*if (steer.GetMagnitudeSquared() > m_turnFactor * m_turnFactor) {
		steer = steer.GetNormalized() * m_turnFactor;
	}*/

	//m_acceleration += steer;
	/*Vec3<float> desiredDir = (m_velocity + m_acceleration);
	if (desiredDir.GetMagnitudeSquared() > EPSILON) {
		desiredDir.Normalize();
	}*/


	m_velocity += m_acceleration * delta;
	//m_velocity = desiredDir * m_maxSpeed;



	//m_velocity *= 0.995f;

	// Finally, limit max speed so they don't accelerate to infinity
	if (m_velocity.GetMagnitudeSquared() > m_maxSpeed * m_maxSpeed) {
		m_velocity = m_velocity.GetNormalized() * m_maxSpeed;
	}
	else if (m_velocity.GetMagnitudeSquared() < m_minSpeed * m_minSpeed) {
		m_velocity = m_velocity.GetNormalized() * m_minSpeed;
	}

	m_position += m_velocity * delta;
	if (m_velocity.GetMagnitudeSquared() > EPSILON) {
		RotateTowards(m_velocity, delta);
	}
}

void Boid::RotateTowards(Vec3<float> targetDir, float delta) {
	if (targetDir.GetMagnitudeSquared() < EPSILON) {
		return;
	}

	Quaternion targetRot = Quaternion::FromTo({ 0, 1, 0 }, targetDir);

	float rotationSpeed = 5.f;
	m_delta = Quaternion::Slerp(m_delta, targetRot, rotationSpeed * delta);
}