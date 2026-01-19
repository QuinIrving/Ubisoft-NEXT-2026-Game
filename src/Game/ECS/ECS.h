#pragma once
#include <cstdint>
#include <vector>
#include "Math/Vec3.h"
#include <algorithm>
#include <unordered_map>
#include <optional>
#include <Graphics/Colour.h>
#include <Graphics/Vertex.h>
#include <Movement/Movement.h>
#include <Graphics/Camera.h>
/*
using CameraComponent = Camera;
using Entity = uint32_t;
constexpr Entity INVALID_ENTITY = 0;

namespace ECS {

    template <typename T>
    struct ComponentPool {
        std::vector<T> data;
        std::vector<Entity> entity;
        std::unordered_map<Entity, size_t> entityToIndex;

        void Add(Entity e, const T& component) {
            size_t idx = data.size();
            data.push_back(component);
            entity.push_back(e);
            entityToIndex[e] = idx;
        }

        std::optional<std::reference_wrapper<T>> Get(Entity e) {
            auto eIdx = entityToIndex.find(e);
            
            if (eIdx == entityToIndex.end()) {
                return std::nullopt;
            }

            return data[eIdx->second];
        }

        void Remove(Entity e) {
            auto eIdx = entityToIndex.find(e);

            if (eIdx == entityToIndex.end()) {
                return;
            }

            size_t idx = eIdx->second;
            size_t last = data.size() - 1;

            if (idx != last) {
                // swap to later pop
                data[idx] = std::move(data[last]);
                entity[idx] = entity[last];
                entityToIndex[entity[last]] = idx;
            }

            data.pop_back();
            entity.pop_back();
            entityToIndex.erase(e);
        }
    };



    struct Transform {
        Vec3<float> position;
        Vec3<float> rotation;
        Vec3<float> scale;
    };

    struct Velocity {
        Vec3<float> linear;
    };

    enum class ColliderType {
        Capsule,
        Sphere,     // player
        AABB,       // walls, spikes
        Heightfield // deformable quads
    };

    struct Heightfield {
        int w, h;
        float cellSize;
        std::vector<float> heights;
        bool dirty = false;
    };

    inline float SampleHeight(const Heightfield& hf, float x, float z) {
        int ix = int(x / hf.cellSize);
        int iz = int(z / hf.cellSize);

        ix = std::clamp(ix, 0, hf.w - 1);
        iz = std::clamp(iz, 0, hf.h - 1);

        return hf.heights[iz * hf.w + ix];
    }

    struct MovementIntent {
        Vec3<float> wishDir;
    };

    struct Boid {
        float aggression;        // builds over time
        float aggressionRadius; // terrain influence radius
        float aggressionRate;   // per second
    };

    struct Collider {
        ColliderType type;
        Vec3<float> offset;

        // capsule
        float radius;
        float height;

        // AABB
        Vec3<float> halfExtents;

        // heightfield
        Heightfield* hf;
    };

    struct RigidBody {
        float mass;          // 0 = static
        bool isKinematic;    // moving platforms
    };

    struct CollisionResponse {
        enum Flags : uint32_t {
            PushPlayer = 1 << 0,
            KillPlayer = 1 << 1,
            Slide = 1 << 2,
            TriggerOnly = 1 << 3
        };
        uint32_t flags;
    };

    struct PlayerTag {};
    struct BulletTag {};
    struct EnemyTag {};
    struct DeformableTag {};

    struct Lifetime {
        float remaining;
    };

    enum CollisionLayer : uint32_t {
        PLAYER = 1 << 0,
        WORLD = 1 << 1,
        ENEMY = 1 << 2,
        BULLET = 1 << 3,
        TRIGGER = 1 << 4
    };

    struct CollisionFilter {
        uint32_t layer;
        uint32_t mask;
    };

    struct Renderable {
        std::vector<Vertex> vertices;  // From your Quad/Obj
        Colour color;  // Or Material if more complex
        // Add indices if needed: std::vector<uint32_t> indices;
        // For Obj models: Load meshes here, or have multiple Renderable per entity if sub-meshes
    };

    struct Movement {
        MovementState state = MovementState::GROUND;
        float offGroundTimer = 0.0f;
    };

    struct World {
        uint32_t nextEntity = 1;

        ComponentPool<Transform> transforms;
        ComponentPool<Velocity> velocities;
        ComponentPool<MovementIntent> intents;

        ComponentPool<Collider> colliders;
        ComponentPool<RigidBody> bodies;
        ComponentPool<CollisionResponse> responses;
        ComponentPool<CollisionFilter> filters;

        ComponentPool<PlayerTag> players;
        ComponentPool<EnemyTag> enemies;
        ComponentPool<BulletTag> bullets;

        ComponentPool<Boid> boids;
        ComponentPool<Lifetime> lifetimes;
       
        ComponentPool<Renderable> renderables;
        ComponentPool<Movement> movements;
        ComponentPool<CameraComponent> cameras;
    };

    inline Entity CreateEntity(World& w) {
        return w.nextEntity++;
    }

    template<typename T>
    void AddComponent(World& w, Entity e, const T& component, ComponentPool<T>& pool) {
        pool.Add(e, component);
    }

    struct Hit {
        float time;
        Vec3<float> normal;
        Entity other;
    };

    inline std::optional<std::reference_wrapper<CameraComponent>> GetPlayerCamera(World& w) {
        if (w.players.entity.empty()) {
            return std::nullopt;
        }

        return w.cameras.Get(w.players.entity[0]);
    }
}*/