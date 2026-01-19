#include "Player.h"

Mat4<float> Player::GetViewMatrix() {
	c.SetPosition(m_position + Vec3<float>(0, capsule.height + capsule.radius, 0));
	return c.GetViewMatrix();
}

// We calculate our wishdir via these two and the input direction given by our keyboard input.
Vec3<float> Player::GetForwardDir() {
	Quaternion qYaw = Quaternion(Vec3<float>(0, 1, 0), c.GetYaw());
	return Vec4<float>(0, 0, -1, 0) * qYaw;
}

Vec3<float> Player::GetRightDir() {
	Quaternion qYaw = Quaternion(Vec3<float>(0, 1, 0), c.GetYaw());
	return Vec4<float>(1, 0, 0, 0) * qYaw;
}

/*
namespace Player {
	void Create(ECS::World& w) {
		Entity playerEnt = ECS::CreateEntity(w);

		w.players.Add(playerEnt, ECS::PlayerTag{});
		w.transforms.Add(playerEnt, ECS::Transform{Vec3<float>(0,0,0), Vec3<float>(0,0,0), Vec3<float>(1,1,1) });
		w.velocities.Add(playerEnt, ECS::Velocity{ Vec3<float>(0,0,0) });
		w.intents.Add(playerEnt, ECS::MovementIntent{ Vec3<float>(0,0,0) });
		w.colliders.Add(playerEnt, ECS::Collider{ ECS::ColliderType::Capsule, Vec3<float>(0,0,0), /*radius* / 0.5f, /*height* / 1.8f });
		w.movements.Add(playerEnt, ECS::Movement{ MovementState::GROUND, 0.0f });
		w.cameras.Add(playerEnt, Camera());
	}

	Mat4<float> Player::GetViewMatrix(ECS::World& w) {
		Entity e = w.players.entity[0];

		auto cOpt = ECS::GetPlayerCamera(w);
		if (!cOpt.has_value()) {
			return Mat4<float>();
		}

		auto& c = cOpt.value().get();

		c.SetPosition(m_position + Vec3<float>(0, -m_eyeHeight, 0));
		return c.GetViewMatrix();
	}

	// We calculate our wishdir via these two and the input direction given by our keyboard input.
	Vec3<float> GetForwardDir(ECS::World& w) {
		auto cOpt = ECS::GetPlayerCamera(w);
		if (!cOpt.has_value()) {
			return { 0, 0, -1 };
		}

		auto& cam = cOpt.value().get();
		/*if (!cam) {
			return { 0, 0, -1 };
		}* /

		//Quaternion qYaw = Quaternion(Vec3<float>(0, 1, 0), c.GetYaw());
		//return Vec4<float>(0, 0, -1, 0) * qYaw;
		return cam.GetForward();
	}

	Vec3<float> GetRightDir(ECS::World& w) {
		auto cOpt = ECS::GetPlayerCamera(w);
		if (!cOpt.has_value()) {
			return { 0, 0, -1 };
		}

		auto& cam = cOpt.value().get();

		/*Quaternion qYaw = Quaternion(Vec3<float>(0, 1, 0), c.GetYaw());
		return Vec4<float>(1, 0, 0, 0) * qYaw;* /
		return cam.GetRight();
		
	}
}*/
