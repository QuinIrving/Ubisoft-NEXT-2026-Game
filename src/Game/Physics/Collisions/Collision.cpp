#include "Collision.h"
#include "Entities/Player/Player.h"
#include "Graphics/Triangle.h"

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
    struct Tri {
        Vec3<float> v0;
        Vec3<float> v1;
        Vec3<float> v2;

        Tri(Vec3<float> v0, Vec3<float> v1, Vec3<float> v2) : v0(v0), v1(v1), v2(v2) {};
    };

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

    /*
    bool SweptRayIntersectTri(const Vec3<float>& start, const Vec3<float>& dir, const Tri& tri, const Vec3<float>& pHalf, Intersection& result) {
        // 1. Calculate Triangle Normal
        Vec3<float> edge1 = tri.v1 - tri.v0;
        Vec3<float> edge2 = tri.v2 - tri.v0;
        Vec3<float> cross = edge1.CrossProduct(edge2);
        Vec3<float> normal;
        if (cross.GetMagnitude() > EPSILON) {
            normal = cross.Normalize();
        }

        float distToInflate = std::abs(normal.x * pHalf.x) +
            std::abs(normal.y * pHalf.y) +
            std::abs(normal.z * pHalf.z);

        float d = normal.DotProduct(tri.v0);
        float playerSide = normal.DotProduct(start) - d;

        // We only care about the side the player is currently on
        float shift = (playerSide > 0) ? distToInflate : -distToInflate;
        float inflatedD = d + shift;

        // 2. Intersection with the inflated plane
        float denom = normal.DotProduct(dir);
        if (std::abs(denom) < 1e-6f) return false;

        float t = (inflatedD - normal.DotProduct(start)) / denom;

        // Check if collision happens within this frame (0 to 1 range)
        if (t < 0.0f || t > 1.0f) return false;

        // 3. Point-in-Triangle Check (with a small "fat" buffer)
        // Project the hit point back to the triangle's actual plane
        Vec3<float> hitPointOnPlane = (start + dir * t) - (normal * shift);

        auto edgeCheck = [&](Vec3<float> a, Vec3<float> b, Vec3<float> p, Vec3<float> n) {
            // Adding a small epsilon (0.01) here prevents phasing through the edges/seams
            return n.DotProduct((b - a).CrossProduct(p - a)) >= -0.01f;
            };

        if (edgeCheck(tri.v0, tri.v1, hitPointOnPlane, normal) &&
            edgeCheck(tri.v1, tri.v2, hitPointOnPlane, normal) &&
            edgeCheck(tri.v2, tri.v0, hitPointOnPlane, normal)) {

            result.time = t;
            result.worldNormal = normal;
            return true;
        }
        return false;
    }*/

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

        Vec3<float> sweepDir = dir;
        

        float tMin = 0.f;
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

                tMax = std::min<float>(tMax, t2);

                // exit with no collision if slab intersection is empty
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
        
        /*if (tMax < 0.f) {
            return false;
        }*/

        Vec3<float> hitNormal = { 0, 0, 0 };
        if (hitAxis == -1) {
            // just make up be our access if we're inside an object
            hitAxis = 1;
        }

        hitNormal[hitAxis] = hitSign;
        result.worldNormal = hitNormal;
        result.time = -FLT_MAX;
        //result.time = std::max<float>(0.f, tMin); // if this is set to -FLT_MAX, it works, most likely as we don't really slide and instead end up setting it to 0

        // we already know the center (origin), and the extent is simply b.max, as the origin is the center
        // check all 3 slabs.
        // See in the book:
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
    float MAX_SLIDEABLE_SLOPE_NORMAL = 0.7f; // ~45 degree angle is the steepest angle which we still consider a "floor"

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

            
            if (c.collisionNormal.y > MAX_SLIDEABLE_SLOPE_NORMAL) {
                p.TransitionMoveState(MovementState::GROUND);
                p.ResetOffGroundTimer();
            }

            // only slide if we're moving towards the plane normal.
            if (velProj < 0.f) {

                // Check if we're at a certain angle, at which point we should treat it as a wall or something?
                // separate our the xz and y components  Iguess.


                Vec3<float> slideDir = c.collisionNormal * velProj;
                Vec3<float> slideVel = currVel - slideDir;

                // snap if ground
                if (c.collisionNormal.y > MAX_SLIDEABLE_SLOPE_NORMAL && slideVel.y < 0) {
                    slideVel.y = 0;
                }
                
                // CS-Surf PORTION:
                float adjust = slideVel.DotProduct(c.collisionNormal);
                if (adjust > 0.f) {
                    slideVel -= (c.collisionNormal * adjust);
                }

                

                p.SetVelocity(slideVel);
            }
        } else if (c.type == EntityType::BRIDGE) {
            Vec3 currVel = p.GetVelocity();
            Vec3 bridgeVel = c.kinematicSurfaceVel;

            // Work in bridge-relative space
            Vec3 relVel = currVel - bridgeVel;

            float velProj = relVel.DotProduct(c.collisionNormal);

            if (c.collisionNormal.y > MAX_SLIDEABLE_SLOPE_NORMAL) {
                p.TransitionMoveState(MovementState::GROUND);
                p.ResetOffGroundTimer();
            }

            if (velProj < 0.f) {

                Vec3 slideDir = c.collisionNormal * velProj;
                Vec3 slideVel = relVel - slideDir;


                bool isFloor = c.collisionNormal.y > MAX_SLIDEABLE_SLOPE_NORMAL;
                bool isStatic = c.kinematicSurfaceVel.GetMagnitudeSquared() < 0.0001f;

                if (isFloor && isStatic && slideVel.y < 0.f) {
                    slideVel.y = 0.f;
                }
                /*
                // Ground snap
                if (c.collisionNormal.y > MAX_SLIDEABLE_SLOPE_NORMAL && slideVel.y < 0.f) {
                    slideVel.y = 0.f;
                }*/

                // CS-style correction
               /* float adjust = slideVel.DotProduct(c.collisionNormal);
                if (adjust > 0.f) {
                    slideVel -= c.collisionNormal * adjust;
                }*/

                // Convert back to world velocity
                p.SetVelocity(slideVel + bridgeVel);
            }
        }
        else if (c.type == EntityType::BOID) {
            // game over.
            w.isPlayerDead = true;
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
        Vec3<float> pHalf = { c.radius, c.height * 0.5f + c.radius, c.radius };

        AABB playerBox = AABB(pPos, pSize);

        //int framesNoIntersect = 0;
        Collide collide;
        collide.hasCollision = false;
        collide.time = FLT_MAX;

        
        // I will think of quad as an extremely thin AABB, Width and height are normal, while the z is the skinny portion.
        for (int idx = 0; idx < w.quads.size(); ++idx) {
            Quad& q = w.quads[idx];

            // since the quad's triangles are all on the same plane, we can simply check our two triangles on the plane.
            auto quadVerts = q.GetVertices();
            Tri Tri1 = Tri(quadVerts[0].GetPosition() * q.GetModelMatrix(), quadVerts[1].GetPosition() * q.GetModelMatrix(), quadVerts[2].GetPosition() * q.GetModelMatrix());
            Tri Tri2 = Tri(quadVerts[3].GetPosition() * q.GetModelMatrix(), quadVerts[4].GetPosition() * q.GetModelMatrix(), quadVerts[5].GetPosition() * q.GetModelMatrix());

            /**/
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
            /*bool didWeIntersect = SweptRayIntersectTri(pLocal, relativeVel, Tri1, delta, i);

            Intersection i2;
            didWeIntersect = SweptRayIntersectTri(pLocal, relativeVel, Tri2, delta, i2) || didWeIntersect;

            if (i2.time < i.time) {
                i = i2;
            }*/
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

        for (int idx = 0; idx < w.cube.GetWalls().size(); ++idx) {
            Wall& q = w.cube.GetWalls()[idx];

            // since the quad's triangles are all on the same plane, we can simply check our two triangles on the plane.
            auto quadVerts = q.baseVerts;
            Tri Tri1 = Tri(quadVerts[0].GetPosition(), quadVerts[1].GetPosition(), quadVerts[2].GetPosition());
            Tri Tri2 = Tri(quadVerts[3].GetPosition(), quadVerts[4].GetPosition(), quadVerts[5].GetPosition());

            Vec3<float> playerHalfExtents = pSize * 0.5f;
            Vec3<float> center = (q.bottomLeft + q.topRight) * 0.5f;
            Vec3<float> wallExtents = {
                std::max<float>(std::abs(q.topRight.x - q.bottomLeft.x) * 0.5f, 0.01f),
                std::max<float>(std::abs(q.topRight.y - q.bottomLeft.y) * 0.5f, 0.01f),
                std::max<float>(std::abs(q.topRight.z - q.bottomLeft.z) * 0.5f, 0.01f)
            };
            Vec3<float> combinedHalfExtents = wallExtents + playerHalfExtents;
            //scaledHalfExtents += pSize * 0.5f;


            OBB quadOBB = OBB(center, combinedHalfExtents, Mat4<float>());

            Vec3<float> startPos = pPos;
            Vec3<float> endPos = (startPos + p.GetVelocity() * delta);

            // Maybe do the relative here, need to see the best method!.

            Vec3<float> pLocal = (pPos - quadOBB.center);
            Vec3<float> endLocal = (endPos - quadOBB.center);
            Vec3<float> relativeVel = (endLocal - pLocal) - Vec3<float>(0, 0, 0);

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
                    collide.collisionNormal = (i.worldNormal);
                }
            }

        }

        for (int idx = 0; idx < w.bridges.size(); ++idx) {
            auto& bridge = w.bridges[idx];
            if (bridge.state == BridgeState::Inactive) {
                continue;
            }

            Vec3<float> bVel = bridge.GetTranslateAxis() * bridge.GetGrowthSpeed(); // Axis * GrowthSpeed
            Vec3<float> relativeVel = (p.GetVelocity() - bVel) * delta;

            auto boxes = bridge.GetCollisionBoxes(pHalf);
            for (auto& box : boxes) {
                Intersection i;
                if (SweptRayIntersectOBB(pPos - box.center, relativeVel, box, delta, i)) {
                    collide.hasCollision = true;

                    if (i.time < collide.time) {
                        collide.time = i.time;
                        collide.type = EntityType::BRIDGE;
                        collide.index = idx;
                        collide.collisionNormal = (i.worldNormal);
                        collide.kinematicSurfaceVel = bVel;
                    }
                }
            }
        }

        Vec3<float> boidSize = { 1.f, 2.f, 1.f };
        Vec3<float> boidHalf = boidSize * 0.5f;
        for (int idx = 0; idx < w.boidSwarm.size(); ++idx) {
            Swarm& s = w.boidSwarm[idx];

            for (int bIdx = 0; bIdx < s.GetBoids().size(); ++bIdx) {
                Boid& b = s.GetBoids()[bIdx];
                Vec3<float> boidPos = b.GetPosition();

                OBB boidOBB(
                    boidPos,
                    boidHalf,
                    b.GetRotationMatrix()
                );

                Intersection i;
                Vec3<float> relativeVel = (p.GetVelocity()); // we already move boids when updating ahead of player handling, not the best solution, but they are grouped so we'll be nice to the player
                Vec3<float> rayOrigin = pPos;

                // Swept direction = velocity (units / second)
                Vec3<float> rayDir = p.GetVelocity();

                bool hit = SweptRayIntersectOBB(
                    rayOrigin - boidOBB.center, // transform into OBB space
                    rayDir,
                    boidOBB,
                    delta,                      // time window
                    i
                );
                if (hit) {
                    collide.hasCollision = true;

                    if (i.time < collide.time) {
                        collide.time = i.time;
                        collide.type = EntityType::BOID;
                        collide.index = bIdx;
                        collide.collisionNormal = i.worldNormal;
                    }
                }
            }
        }

        return collide;
    }
}