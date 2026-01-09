#include "ViewVertex.h"

ViewVertex ViewVertex::EdgeMidpointInterpolate(ViewVertex& a, ViewVertex& b) {
	// Simply (a + b) * 0.5f; for each section (except normals).

	// for my normals and tangent, need to simply do (a + b).normalize()
	Vec3<float> worldPosition = (a.GetWorldPosition() + b.GetWorldPosition()) * 0.5f;
	Vec3<float> viewPosition = (a.GetViewPosition() + b.GetViewPosition()) * 0.5f;

	Vec3<float> worldNormal = (a.GetWorldNormal() + b.GetWorldNormal()).Normalize();
	Vec3<float> viewNormal = (a.GetViewNormal() + b.GetViewNormal()).Normalize();

	//Vec3<float> viewTangent = (a.GetViewTangent() + b.GetViewTangent()).Normalize();

	Vec2<float> UV = (a.GetUV() + b.GetUV()) * 0.5f;
	Vec4<float> colour = (a.GetColour() + b.GetColour()) * 0.5f;

	//uint32_t meshIndex = 0; // at this point don't think it matters
	//uint32_t uniqueIndex = 0; // same as above

	return ViewVertex(worldPosition, viewPosition, worldNormal, viewNormal, /*viewTangent*/{}, UV, colour, 0, a.GetMaterialIndex(), 0);
}