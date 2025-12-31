#include "Quad.h"

/*Quad::Quad(float width, float height) {
	vertices.reserve(4);
	vertexIds.reserve(6);

	float wOver2 = width / 2.f;
	float hOver2 = height / 2.f;

	//vertices.push_back({ -wOver2, -hOver2, -1, {0, 0, 1}, {0, 0} });
	m_vertices.push_back(Vertex({ -wOver2, -hOver2, -1 }, {}, { 0, 0, 1 }, {0, 0}));
	vertices.push_back({ wOver2, -hOver2, -1, {0, 0, 1}, {1, 0} });
	vertices.push_back({ -wOver2, hOver2,-1, {0, 0, 1}, {0, 1} });
	vertices.push_back({ wOver2, hOver2, -1, {0, 0, 1}, {1, 1} });

	vertexIds = { 0, 2, 1, 1, 2, 3 };

	for (int i = 0; i < vertexIds.size(); i += 3) {
		Vec3<float> edge1 = vertices[vertexIds[i + 1]].GetPosition() - vertices[vertexIds[i]].GetPosition();
		Vec3<float> edge2 = vertices[vertexIds[i + 2]].GetPosition() - vertices[vertexIds[i]].GetPosition();

		Vec2<float> dUV1 = vertices[vertexIds[i + 1]].GetUV() - vertices[vertexIds[i]].GetUV();
		Vec2<float> dUV2 = vertices[vertexIds[i + 2]].GetUV() - vertices[vertexIds[i]].GetUV();

		float f = 1.0f / (dUV1.x * dUV2.y - dUV2.x * dUV1.y);

		Vec3<float> tangent = (edge1 * dUV2.y - edge2 * dUV1.y) * f;
		tangent = tangent.GetNormalized();
		Vec3<float> bitangent = (edge1 * -dUV2.x + edge2 * dUV1.x) * f;
		bitangent = bitangent.GetNormalized();

		vertices[vertexIds[i]].SetTangent(tangent);
		vertices[vertexIds[i]].SetTangentW((vertices[vertexIds[i]].GetNormal().CrossProduct(tangent).DotProduct(bitangent)) < 0.0f ? -1.0f : 1.0f);

		vertices[vertexIds[i + 1]].SetTangent(tangent);
		vertices[vertexIds[i + 1]].SetTangentW((vertices[vertexIds[i + 1]].GetNormal().CrossProduct(tangent).DotProduct(bitangent)) < 0.0f ? -1.0f : 1.0f);

		vertices[vertexIds[i + 2]].SetTangent(tangent);
		vertices[vertexIds[i + 2]].SetTangentW((vertices[vertexIds[i + 2]].GetNormal().CrossProduct(tangent).DotProduct(bitangent)) < 0.0f ? -1.0f : 1.0f);
	}
}*/