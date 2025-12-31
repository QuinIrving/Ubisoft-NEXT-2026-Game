#include "Quad.h"

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

ModelAttributes Quad::GetModelAttributes() {
	ModelAttributes ma;
	ma.material = material;
	ma.modelMatrix = Mat4<float>::Scale(m_scale) * m_delta.GetRotationMatrix() * Mat4<float>::Translate(m_position);
	return ma;
}

std::vector<uint32_t>& Quad::GetVertexIds() {
	return m_mesh.GetVertexIds();
}

std::vector<Vertex>& Quad::GetVertices() {
	return m_mesh.GetVertices();
}