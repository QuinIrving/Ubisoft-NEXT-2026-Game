#include "Model.h"

// Rotate around axis
void Model::Rotate(float x, float y, float z) {
	m_delta *= Quaternion(x, y, z);
	m_delta.Normalize();
}

void Model::Scale(float x, float y, float z) {
	m_scale *= Vec3<float>(x, y, z);
}

void Model::Translate(float x, float y, float z) {
	m_position += Vec3<float>(x, y, z);
}

Mat4<float> Model::GetModelMatrix() const {
	return Mat4<float>::Scale(m_scale) * m_delta.GetRotationMatrix() * Mat4<float>::Translate(m_position);
}

/*
const ModelAttributes& Model::GetAttributes() {
	attributes.meshes = m_meshes;
	attributes.modelMatrix = Mat4<float>::Scale(m_scale) * m_delta.GetRotationMatrix() * Mat4<float>::Translate(m_position);
}*/