#include "Quad.h"
#include "Tessellation/TriangleNode.h"

Quad::Quad(float width, float height, float size, Vec4<float> colour) {
	Scale(size, size, size);
	m_mesh = QuadMesh(width, height, colour);
}

void Quad::Rotate(float x, float y, float z) {
	//m_rotation.x = std::fmodf(m_rotation.x + x, 360.f);
	//m_rotation.y = std::fmodf(m_rotation.y + y, 360.f);
	//m_rotation.z = std::fmodf(m_rotation.z + z, 360.f);
	Quaternion deltaRotation = Quaternion(x, y, z);

	m_delta *= deltaRotation;
	m_delta.Normalize();
}

void Quad::Scale(float x, float y, float z) {
	m_scale.x *= x;
	m_scale.y *= y;
	m_scale.z *= z;
}

void Quad::Translate(float x, float y, float z) {
	m_position.x += x;
	m_position.y += y;
	m_position.z += z;
}

Mat4<float> Quad::GetModelMatrix() const {
	return Mat4<float>::Scale(m_scale) * m_delta.GetRotationMatrix() * Mat4<float>::Translate(m_position);
}

std::vector<uint32_t>& Quad::GetVertexIds() {
	return m_mesh.GetVertexIds();
}

std::vector<Vertex>& Quad::GetVertices() {
	return m_mesh.GetVertices();
}

Mesh Quad::GetMesh() {
	Mesh m;
	m.geometry = std::make_shared<MeshGeometry>(MeshGeometry(m_mesh.GetVertices()));
	m.material = material;
	
	return m;
}

ModelEdge Quad::GetAdjacencyTable() {
	ModelEdge me;

	auto key = MakeEdgeKey(0, 1);
	me.adjacencyTable[key].push_back(0);


	key = MakeEdgeKey(1, 2);
	me.adjacencyTable[key].push_back(0);
	me.adjacencyTable[key].push_back(1);

	key = MakeEdgeKey(0, 2);
	me.adjacencyTable[key].push_back(0);


	key = MakeEdgeKey(1, 3);
	me.adjacencyTable[key].push_back(1);

	key = MakeEdgeKey(2, 3);
	me.adjacencyTable[key].push_back(1);
	return me;
}