#include "Collision.h"
#include "Entities/Player/Player.h"
#include "Graphics/Triangle.h"

/*
void World::Update(float dt) {
    player.Update(dt);

    ResolvePlayerWorldCollision();
    ResolvePlayerQuadCollision();
    ResolvePlayerEnemyCollision();

    for (auto& e : enemies)
        e.Update(dt, player);

    ResolveEnemyWorldCollision();
}

void ResolvePlayerWorldCollision() {
    Vec3 pos = player.GetPosition();
    float r = player.GetRadius();

    pos.x = std::clamp(pos.x, worldBounds.min.x + r, worldBounds.max.x - r);
    pos.y = std::clamp(pos.y, worldBounds.min.y + r, worldBounds.max.y - r);
    pos.z = std::clamp(pos.z, worldBounds.min.z + r, worldBounds.max.z - r);

    player.SetPosition(pos);
}
}*/

struct Sweep {
    float time;  // Non-negative time of first contact.
    float depth; // Non-negative penetration depth if objects start initially colliding.
    Vec3<float> point;  // Point of first-contact. Only updated when contact occurs.
    Vec3<float> normal; // Unit-length collision normal. Only updated when contact occurs.
};





struct AABB {
    Vec3<float> min;
    Vec3<float> max;

    AABB(Vec3<float> pos, Vec3<float> size) {
        Vec3<float> halfSize = size / 2.f;
        min = pos - halfSize;
        max = pos + halfSize;
    }

    Vec3<float> GetCenter() const {
        return (min + max) * 0.5f;
    }

    void SetCenter(Vec3<float> pos) {
        Vec3<float> half = (max - min) * 0.5f;
        min = pos - half;
        max = pos + half;
    }
};

struct OBB {
    Vec3<float> center; // model matrix translation
    Vec3<float> halfEdges; // quad width/height/depth / 2
    Mat4<float> rotation; // need to extract the upper 3x3 corner, and divide it by the scale factor the object already has.

    OBB(Vec3<float> c, Vec3<float> he, Mat4<float> r) : center(c), halfEdges(he), rotation(r) {};
};

/*
struct Collision {
    AABB& other;
    float time;
};*/

struct Intersection {
    float time;
    Vec3<float> worldNormal;
};


namespace {
    bool Intersects(const AABB& a, const AABB& b) {
        bool xNotAligned = (a.max.x < b.min.x || a.min.x > b.max.x);
        bool yNotAligned = (a.max.y < b.min.y || a.min.y > b.max.y);
        bool zNotAligned = (a.max.z < b.min.z || a.min.z > b.max.z);

        return !(xNotAligned || yNotAligned || zNotAligned);
    }

    Vec3<float> GetPenetration(const AABB& a, const AABB& b) {
        // positive overlap depths on each axis.
        float dx = std::min<float>(a.max.x - b.min.x, b.max.x - a.min.x);
        float dy = std::min<float>(a.max.y - b.min.y, b.max.y - a.min.y);
        float dz = std::min<float>(a.max.z - b.min.z, b.max.z - a.min.z);
        return { dx, dy, dz };
    }

    bool SweptRayIntersectOBB(const Vec3<float>& point, const Vec3<float>& dir, const OBB& box, float delta, Intersection& result) {
        // slab = space between 2 paralell lines
        // box seen as intersection of 3 such slabs.
        // basically whats in "intersects"
        /*
        If the farthest entry ever becomes farther
        than the nearest exit, the ray cannot be intersecting the slab intersection volume and
        the test can exit early (with a result of nonintersection)

        Ray equation: R(t) = p + t*d, against AABB a.
        so Ray point at time t = start point p, + time * direction? 
        we can then return the intersection distance tmin and point q of intersection


        */

        /*
        Big note, we already translated and did all of our necessary transformations of point and dir, so the box center should be at the origin.
        */

        Vec3<float> min = -box.halfEdges;
        Vec3<float> max = box.halfEdges;

        Vec3<float> sweepDir = dir;// * delta;

        float tMin = 0.f;
        //float tMin = -FLT_MAX;
        float tMax = 1.f;

        int hitAxis = -1; 
        float hitSign = 0.0f;

        for (int i = 0; i < 3; ++i) {
            if (std::fabsf(sweepDir[i]) < EPSILON) {
                // Ray is parallel to slab, therefore not a hit if our point is not within the slab
                if (point[i] < min[i] || point[i] > max[i]) {
                    return false;
                }
            }
            else {
                // compute intersection t val of ray with near and far plane of slab
                float dirInv = 1.f / sweepDir[i];
                float t1 = (min[i] - point[i]) * dirInv;
                float t2 = (max[i] - point[i]) * dirInv;

                // Make t1 intersection with near plane, t2 with far plane.
                if (t1 > t2) {
                    std::swap(t1, t2);
                }

                if (t1 > tMin) {
                    tMin = t1;
                    hitAxis = i;
                    hitSign = (dirInv < 0.0f) ? 1.f : -1.f;
                }

                /*if (t2 > tMax) {
                    tMax = t2;
                }*/

                tMax = std::min<float>(tMax, t2);

                // exit with no collision if slab intersection is empty (
                if (tMin > tMax) {
                    return false;
                }
            }

        }

        // ray must intersect with all 3 slabs. return point q, and intersection t value.
        Vec3<float> q = point + sweepDir * tMin;

        /*
        if (tMin < 0) {
            result.time = 0.f;
        }
        else {
            result.time = tMin;
        }*/

        Vec3<float> hitNormal = { 0, 0, 0 };
        if (hitAxis == -1) {
            // just make up be our access if we're inside an object
            hitAxis = 1;
        }
        hitNormal[hitAxis] = hitSign;
        result.worldNormal = hitNormal;

        // we already know the center (origin), and the extent is simply b.max, as the origin is the center


        // check all 3 slabs.
        // See oin the book:
        // It has the projection interval radius r_b is given by it equals
        // (halfEdge0 * abs(localV.x) + halfEdge1 * abs(localV.y) + halfEdge2 * abs(localV.z) ) / magnitude of localV.

        /*
        There are size axes that must be tested as separating axes. 3 of the AABB normals: (1, 0, 0), (0, 1, 0), (0, 0, 1).

        and 3 corresponding the corssproducts between segment dir vector, and face norma;s
        v3 = dir.CrossProduct({1, 0, 0}), v4 = dir.CrossProduct({0, 1, 0}), v5 = dir.CrossProduct({0, 0, 1})

        Table has way to compute the d_s, r_b, r_s, |d_s| > r_b + r_s
        */

        return true;
    }

    
}

namespace Collision {
    float MAX_WALKABLE_SLOPE_NORMAL = 0.7f; // ~45 degree angle is the steepest angle which we still consider a "floor"

    void ResolvePlayerCollision(World& w, float delta) {

        int MAX_ITERATIONS = 3;
        int iteration = 0;
        Collide c;

        float frameTime = delta;

        do {
            c = DetectPlayerCollision(w, frameTime);
            
            if (c.hasCollision) {
                HandlePlayerCollisionResolution(w, c, frameTime);
            }

            iteration++;
        } while (c.hasCollision && frameTime > 0.f && iteration < MAX_ITERATIONS);

        // resolve, and keep iterating.
    }

    void Collision::HandlePlayerCollisionResolution(World& w, Collide& c, float& delta) {
        Player& p = w.player;

        if (c.type == EntityType::QUAD) {
            Vec3<float> currVel = p.GetVelocity();
            
            float moveTime = std::max<float>(c.time - 0.01f, 0.0f);
            p.UpdatePosition(currVel * moveTime * delta);
            delta -= (delta * moveTime);

            float velProj = currVel.DotProduct(c.collisionNormal);

            
            if (c.collisionNormal.y > MAX_WALKABLE_SLOPE_NORMAL) {
                p.TransitionMoveState(MovementState::GROUND());
                p.ResetOffGroundTimer();
            }

            // only slide if we're moving towards the plane normal.
            if (velProj < 0.f) {

                // Check if we're at a certain angle, at which point we should treat it as a wall or something?
                // separate our the xz and y components  Iguess.


                Vec3<float> slideDir = c.collisionNormal * velProj;
                Vec3<float> slideVel = currVel - slideDir;

                
                // snap if ground
                if (c.collisionNormal.y > MAX_WALKABLE_SLOPE_NORMAL && slideVel.y < 0) {
                    slideVel.y = 0;
                }

                // CS-Surf PORTION:
                float adjust = slideVel.DotProduct(c.collisionNormal);
                if (adjust > 0.f) {
                    slideVel -= (c.collisionNormal * adjust);
                }

                p.SetVelocity(slideVel);
            }
        }

        // can handle different types in here.
    }


    Collide Collision::DetectPlayerCollision(World& w, float delta) {
        Player& p = w.player;
        CapsuleCollider c = p.GetCollider();

        //Vec3<float> playerBL = p.GetPosition() + Vec3<float>(-c.radius, 0, c.radius);
        //Vec3<float> playerTR = p.GetPosition() + Vec3<float>(c.radius, c.height + (2 * c.radius), -c.radius);

        Vec3<float> pPos = p.GetPosition() + Vec3<float>(0, c.radius + (c.height / 2), 0);
        Vec3<float> pSize = { c.radius * 2, c.radius * 2 + c.height, c.radius * 2 };

        AABB playerBox = AABB(pPos, pSize);

        //int framesNoIntersect = 0;
        Collide collide;
        collide.hasCollision = false;
        collide.time = FLT_MAX;

        //OutputDebugString("\n");
        // I will think of quad as an extremely thin AABB, Width and height are normal, while the z is the skinny portion.
        for (int idx = 0; idx < w.quads.size(); ++idx) {
            Quad& q = w.quads[idx];

            // since the quad's triangles are all on the same plane, we can simply check our two triangles on the plane.
            auto quadVerts = q.GetVertices();

            auto qScale = q.GetScale(); // for extending half-extents
            // m_delta.GetRotationMatrix()
            // Vec3<float> GetTranslation() { return m_position; }
            auto qTranslate = q.GetTranslation(); // for center position of quad. Technically since quad's center starts at origin, this should always simply be the center pos.

            // apply scale then translate? Can create the correct matrices if needed.

            auto qRot = q.GetRotationMatrix(); // to put our point back to world space.
            auto qRotInverse = qRot.GetTranspose(); // to put our player to OBB space

            //GetLocalHalfExtent for our quad extents, gives the width, height, and epsilon size, before scale is applied.
            

            Vec3<float> localXAxis = { qRotInverse[0][0], qRotInverse[1][0], qRotInverse[2][0] };
            Vec3<float> localYAxis = { qRotInverse[0][1], qRotInverse[1][1], qRotInverse[2][1] };
            Vec3<float> localZAxis = { qRotInverse[0][2], qRotInverse[1][2], qRotInverse[2][2] };

            Vec3<float> playerHalfExtents = pSize * 0.5f;

            // 3. Project Player AABB onto the Quad's Local Axes
            // We take the absolute dot product to get the maximum extent in that direction
            float addedX = std::abs(localXAxis.x * playerHalfExtents.x) +
                std::abs(localXAxis.y * playerHalfExtents.y) +
                std::abs(localXAxis.z * playerHalfExtents.z);

            float addedY = std::abs(localYAxis.x * playerHalfExtents.x) +
                std::abs(localYAxis.y * playerHalfExtents.y) +
                std::abs(localYAxis.z * playerHalfExtents.z);

            float addedZ = std::abs(localZAxis.x * playerHalfExtents.x) +
                std::abs(localZAxis.y * playerHalfExtents.y) +
                std::abs(localZAxis.z * playerHalfExtents.z);


            Vec3<float> scaledHalfExtents = q.GetLocalHalfExtent() * q.GetScale();
            // Minkowski skum so we become a point instead of an AABB
            scaledHalfExtents += Vec3<float>(addedX, addedY, addedZ);
            //scaledHalfExtents += pSize * 0.5f;


            OBB quadOBB = OBB(qTranslate, scaledHalfExtents, qRot);

            Vec3<float> startPos = pPos;
            Vec3<float> endPos = (startPos + p.GetVelocity() * delta);

            // Maybe do the relative here, need to see the best method!.

            Vec3<float> pLocal = (pPos - quadOBB.center) * qRotInverse;
            Vec3<float> endLocal = (endPos - quadOBB.center) * qRotInverse;
            //Vec3<float> velRelative = p.GetVelocity() - quad.GetVelocity(); // for whenever we have non-static geometry.
            Vec3<float> relativeVel = (endLocal - pLocal) - Vec3<float>(0, 0, 0);
            //Vec3<float> vLocal = relativeVel * qRotInverse;

            

            // now we have a ray, at start position pLocal, with direction vLocal.
            Intersection i;
            bool didWeIntersect = SweptRayIntersectOBB(pLocal, relativeVel, quadOBB, delta, i);

            if (didWeIntersect) {
                // rotate it to the correct spot.
                //i.worldNormal = i.worldNormal * qRot;
                collide.hasCollision = true;
                
                if (i.time < collide.time) {
                    collide.time = i.time;
                    collide.type = EntityType::QUAD;
                    collide.index = idx;
                    collide.collisionNormal = (i.worldNormal * qRot);
                }
            }

        }

        return collide;
    }
}