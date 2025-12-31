#include "QuadMesh.h"
#include "Graphics/Triangle.h"

QuadMesh::QuadMesh(float width, float height, Vec4<float> colour) {
	m_vertices.reserve(4);
	m_vertexIds.reserve(6);

	float wOver2 = width / 2.f;
	float hOver2 = height / 2.f;

	m_vertices.push_back(Vertex({ -wOver2, -hOver2, 0 }, colour, { 0, 0, 1 }, {0, 0}));
	m_vertices.push_back(Vertex({ wOver2, -hOver2, 0 }, colour, { 0, 0, 1 }, { 1, 0 }));
	m_vertices.push_back(Vertex({ -wOver2, hOver2, 0 }, colour, { 0, 0, 1 }, { 0, 1 }));
	m_vertices.push_back(Vertex({ wOver2, hOver2, 0 }, colour, { 0, 0, 1 }, { 1, 1 }));

	m_vertexIds = { 0, 2, 1, 1, 2, 3 };

	for (int i = 0; i < m_vertexIds.size(); i += 3) {
		Vec3<float> edge1 = m_vertices[m_vertexIds[i + 1]].GetPosition() - m_vertices[m_vertexIds[i]].GetPosition();
		Vec3<float> edge2 = m_vertices[m_vertexIds[i + 2]].GetPosition() - m_vertices[m_vertexIds[i]].GetPosition();

		Vec2<float> dUV1 = m_vertices[m_vertexIds[i + 1]].GetUV() - m_vertices[m_vertexIds[i]].GetUV();
		Vec2<float> dUV2 = m_vertices[m_vertexIds[i + 2]].GetUV() - m_vertices[m_vertexIds[i]].GetUV();

		float f = 1.0f / (dUV1.x * dUV2.y - dUV2.x * dUV1.y);

		Vec3<float> tangent = (edge1 * dUV2.y - edge2 * dUV1.y) * f;
		tangent = tangent.GetNormalized();
		Vec3<float> bitangent = (edge1 * -dUV2.x + edge2 * dUV1.x) * f;
		bitangent = bitangent.GetNormalized();

		m_vertices[m_vertexIds[i]].SetTangent(tangent);
		m_vertices[m_vertexIds[i]].SetTangentW((m_vertices[m_vertexIds[i]].GetNormal().CrossProduct(tangent).DotProduct(bitangent)) < 0.0f ? -1.0f : 1.0f);

		m_vertices[m_vertexIds[i + 1]].SetTangent(tangent);
		m_vertices[m_vertexIds[i + 1]].SetTangentW((m_vertices[m_vertexIds[i + 1]].GetNormal().CrossProduct(tangent).DotProduct(bitangent)) < 0.0f ? -1.0f : 1.0f);

		m_vertices[m_vertexIds[i + 2]].SetTangent(tangent);
		m_vertices[m_vertexIds[i + 2]].SetTangentW((m_vertices[m_vertexIds[i + 2]].GetNormal().CrossProduct(tangent).DotProduct(bitangent)) < 0.0f ? -1.0f : 1.0f);
	}
}

void QuadMesh::SetVertexNormals() {
	for (int i = 0; i < m_vertexIds.size(); i += 3) {
		Vec3<float> faceNormal = Triangle::ComputeFaceNormal(m_vertices[m_vertexIds[i]].GetPosition(), m_vertices[m_vertexIds[i + 1]].GetPosition(), m_vertices[m_vertexIds[i + 2]].GetPosition());

		m_vertices[m_vertexIds[i]].SetNormal(faceNormal);
		m_vertices[m_vertexIds[i + 1]].SetNormal(faceNormal);
		m_vertices[m_vertexIds[i + 2]].SetNormal(faceNormal);
	}
}