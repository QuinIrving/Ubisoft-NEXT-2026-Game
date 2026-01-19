#include "Bridge.h"
#include "Physics/Collisions/Colliders.h"

namespace {
	Vec3<float> GetPointsExtents(const std::vector<Vertex>& verts) {
		if (verts.empty()) {
			return { 0.01f, 0.01f, 0.01f };
		}

		Vec3<float> min = verts[0].GetPosition();
		Vec3<float> max = verts[0].GetPosition();

		for (const auto& v : verts) {
			Vec3<float> p = v.GetPosition();
			if (p.x < min.x) {
				min.x = p.x;
			}

			if (p.x > max.x) {
				max.x = p.x;
			}

			if (p.y < min.y) { 
				min.y = p.y; 
			} 
			
			if (p.y > max.y) {
				max.y = p.y;
			}

			if (p.z < min.z) { 
				min.z = p.z;
			} 
			
			if (p.z > max.z) {
				max.z = p.z;
			}

		}

		Vec3<float> halfEdges = (max - min) * 0.5f;

		if (halfEdges.x < 0.001f) {
			halfEdges.x = 0.01f;
		}

		if (halfEdges.y < 0.001f) {
			halfEdges.y = 0.01f;
		}

		if (halfEdges.z < 0.001f) {
			halfEdges.z = 0.01f;
		}

		return halfEdges;
	}

	Vec3<float> GetPointsCenter(const std::vector<Vertex>& verts) {
		if (verts.empty()) return { 0, 0, 0 };

		Vec3<float> sum = { 0, 0, 0 };
		for (const auto& v : verts) {
			sum += v.GetPosition();
		}

		// Average position
		return sum / static_cast<float>(verts.size());
	}
}

void Bridge::Update(float delta, float maxLength) {
	switch (state) {
	case BridgeState::Expanding: {
		// Grow along extension axis
		translation += (translateAxis * growthSpeed * delta);
		
		if (translation.GetMagnitudeSquared() >= maxLength * maxLength) {
			translation = translateAxis * maxLength;//translation.GetNormalized() * maxLength;
			state = BridgeState::FullyExtended;
			timeElapsedStatic = 0.f;
		}

		break;
	}

	case BridgeState::Contracting: {
		translation -= (translateAxis * growthSpeed * delta);

		if (translation.DotProduct(translateAxis) <= 0.f) {
			translation = { 0, 0, 0 };
			state = BridgeState::Inactive;
			timeElapsedStatic = 0.f;
		}

		break;
	}

	case BridgeState::FullyExtended: {
		timeElapsedStatic += delta;

		if (timeElapsedStatic >= timeUntilTransition) {
			state = BridgeState::Contracting;
			timeElapsedStatic = 0.f;
		}

		break;
	}
	case BridgeState::Inactive: {
		timeElapsedStatic += delta;

		if (timeElapsedStatic >= timeUntilTransition) {
			state = BridgeState::Expanding;
			timeElapsedStatic = 0.f;
		}

		break;
	}
	}
}

Bridge::Bridge(Vec3<float> bottomLeft, Vec3<float> bottomRight, Vec3<float> topCenter, BridgeClass type, Vec3<float> translateAxis, float secBetweenTransitions, float growthSpeed)
	: type(type), translateAxis(translateAxis), timeUntilTransition(secBetweenTransitions), growthSpeed(growthSpeed) {

	auto addSide = [&](Vec3<float> p1, Vec3<float> p2, Vec3<float> norm) {
		KinematicWall w;
		w.normal = norm;
		w.isCapFace = false;
		w.staticVerts = { Vertex(p1), Vertex(p2) }; // The anchor line
		walls.push_back(w);
	};

	auto addCap = [&](std::vector<Vec3<float>> pts, Vec3<float> norm) {
		KinematicWall w;
		w.normal = norm;
		w.isCapFace = true;
		// Build out our cap face.
		if (pts.size() == 3) { // Triangle
			w.staticVerts = { Vertex(pts[0]), Vertex(pts[1]), Vertex(pts[2]) };
		}
		else { // Quad
			w.staticVerts = { Vertex(pts[0]), Vertex(pts[1]), Vertex(pts[2]),
							  Vertex(pts[0]), Vertex(pts[2]), Vertex(pts[3]) };
		}
		walls.push_back(w);
	};
	
	if (type == BridgeClass::Box) {
		Vec3<float> topLeft = { bottomLeft.x, topCenter.y, bottomLeft.z };
		Vec3<float> topRight = { bottomRight.x, topCenter.y, bottomRight.z };

		// 4 sides of the box
		addSide(bottomLeft, bottomRight, { 0, -1, 0 }); // Floor
		addSide(topLeft, topRight, { 0, 1, 0 });       // Ceiling
		addSide(bottomLeft, topLeft, { -1, 0, 0 });    // Left
		addSide(bottomRight, topRight, { 1, 0, 0 });   // Right

		// The front face
		addCap({ bottomLeft, bottomRight, topRight, topLeft }, { 0, 0, 1 });
	}
	else if (type == BridgeClass::TriangularPrism) {
		// 3 sides of the prism
		addSide(bottomLeft, bottomRight, { 0, -1, 0 }); // Floor
		addSide(bottomLeft, topCenter, { -1, 1, 0 });   // Left Slope
		addSide(bottomRight, topCenter, { 1, 1, 0 });   // Right Slope

		// The front triangle
		addCap({ bottomLeft, bottomRight, topCenter }, { 0, 0, 1 });
	}
}

std::vector<Vertex> Bridge::GetVertices() {
	std::vector<Vertex> combinedVerts;
	combinedVerts.reserve(GetWalls().size() * 2 * 3); // 2 triangles per quad, 3 verts per tri.

	for (auto& wall : walls) {
		if (wall.isCapFace) {
			// Our end cap, means teach vertex needs to move forward as a solid face.
			for (const auto& vTemplate : wall.staticVerts) {
				Vec3<float> newPos = vTemplate.GetPosition() + translation;
				
				// shift Position
				combinedVerts.push_back(Vertex(newPos, vTemplate.GetColour(), wall.normal));
			}

			continue;
		}

		// handling side walls so they need to stretch
		Vertex v0 = wall.staticVerts[0]; // These are our static anchors
		Vertex v1 = wall.staticVerts[1];

		Vertex v0_K = Vertex(v0.GetPosition() + translation, v0.GetColour(), wall.normal, { 1, 0 });
		Vertex v1_K = Vertex(v1.GetPosition() + translation, v1.GetColour(), wall.normal, { 1, 1 });

		// Tri 1: Static A -> Kinematic A -> Kinematic B
		combinedVerts.push_back(v0);
		combinedVerts.push_back(v0_K);
		combinedVerts.push_back(v1_K);

		// Tri 2: Static A -> Kinematic b -> Static b
		combinedVerts.push_back(v0);
		combinedVerts.push_back(v1_K);
		combinedVerts.push_back(v1);
	}

	return combinedVerts;
}

std::vector<OBB> Bridge::GetCollisionBoxes(Vec3<float> playerHalfSize) {
	std::vector<OBB> boxes;
	float currLen = translation.GetMagnitude();

	if (currLen < 0.001f) {
		return boxes;
	}

	for (auto& wall : walls) {
		if (wall.isCapFace) {
			// front face, all kinematic
			// Calculate center by taking the original cap center and adding translation
			Vec3<float> originalCenter = GetPointsCenter(wall.staticVerts);
			Vec3<float> currentCenter = originalCenter + translation;

			Vec3<float> extents = GetPointsExtents(wall.staticVerts);

			// Minkowski Sum
			boxes.push_back(OBB(currentCenter, extents + playerHalfSize, Mat4<float>()));
		}
		else {
			// Our quads that stretch with static points
			Vec3<float> anchorCenter = (wall.staticVerts[0].GetPosition() + wall.staticVerts[1].GetPosition()) * 0.5f;
			Vec3<float> currentCenter = anchorCenter + (translation * 0.5f);
			Vec3<float> baseExtents = GetPointsExtents(wall.staticVerts);

			Vec3<float> growth = {
				std::abs(translation.x * 0.5f),
				std::abs(translation.y * 0.5f),
				std::abs(translation.z * 0.5f)
			};

			boxes.push_back(OBB(currentCenter, baseExtents + growth + playerHalfSize, Mat4<float>()));
		}
	}

	return boxes;
}