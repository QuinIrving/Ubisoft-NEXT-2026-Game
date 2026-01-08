#pragma once
#include <vector>
#include "Mesh.h"
#include "Math/Vec3.h"
#include "Math/Quaternion.h"
#include "Math/MatVecQuat_ops.h"
#include "Graphics/ModelAttributes.h"

// Generic Class for a model that represents 1 or more meshes from a .obj file.
// Probably want a hashmap to retrieve our model and make a copy
class Model {
public:
	// We do need to retrieve our "Model attributes" which should include our Model matrix, and our meshes (which contain their material usage)
	// Need to find a way to nicely integrate it into our pipeline (or make it so that quad can coexist with it as well)
	Model(std::vector<Mesh> meshes) : m_meshes(meshes) {};

	// Rotate around axis
	void Rotate(float x, float y, float z);
	void Scale(float x, float y, float z);
	void Scale(float factor) { Scale(factor, factor, factor); }
	void Translate(float x, float y, float z);

	//const ModelAttributes& GetAttributes();
	Mat4<float> GetModelMatrix() const;
	std::vector<Mesh>& GetMeshes() { return m_meshes; }

private:
	Vec3<float> m_position{ 0, 0, 0 };
	Vec3<float> m_scale{1, 1, 1};
	Quaternion m_delta;
	std::vector<Mesh> m_meshes;
	//ModelAttributes attributes;
};