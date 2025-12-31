#pragma once
#include <vector>
#include <Graphics/Vertex.h>
#include <Graphics/ModelAttributes.h>

class Quad {
public:
	//Quad(float width = 1.f, float height = 1.f, Vec4<float> colour = {1.f, 1.f, 1.f, 1.f}); // Perhaps colour should be something in our material idea

	std::vector<Vertex>& GetVertices() { return m_vertices; }
	std::vector<uint32_t>& GetVertexIds() { return m_vertexIds; }

	//void SetVertexNormals();

	Material m_material;

private:
	std::vector<Vertex> m_vertices;
	std::vector<uint32_t> m_vertexIds;
};