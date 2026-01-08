#pragma once
#include <vector>
#include <Graphics/Vertex.h>
#include <Graphics/ModelAttributes.h>

class QuadMesh /* : public Mesh*/ {
public:
	QuadMesh(float width = 1.f, float height = 1.f, Vec4<float> colour={1.f, 1.f, 1.f, 1.f});

	std::vector<Vertex>& GetVertices() { return m_vertices; }
	std::vector<uint32_t>& GetVertexIds() { return m_vertexIds; }

	void SetVertexNormals();

private:
	std::vector<Vertex> m_vertices;
	std::vector<uint32_t> m_vertexIds;
};