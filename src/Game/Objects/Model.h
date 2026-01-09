#pragma once
#include <vector>
#include "Mesh.h"
#include "Math/Vec3.h"
#include "Math/Quaternion.h"
#include "Math/MatVecQuat_ops.h"
#include "Graphics/ModelAttributes.h"

struct ModelEdge {
	std::unordered_map<uint64_t, std::vector<uint32_t>> adjacencyTable;
};

// Generic Class for a model that represents 1 or more meshes from a .obj file.
// Probably want a hashmap to retrieve our model and make a copy
class Model {
public:
	Model() = default;
	// We do need to retrieve our "Model attributes" which should include our Model matrix, and our meshes (which contain their material usage)
	// Need to find a way to nicely integrate it into our pipeline (or make it so that quad can coexist with it as well)
	Model(std::vector<Mesh> meshes, ModelEdge edges) : m_meshes(meshes), m_edges(std::make_shared<ModelEdge>(edges)) {};

	// Rotate around axis
	void Rotate(float x, float y, float z);
	void Scale(float x, float y, float z);
	void Scale(float factor) { Scale(factor, factor, factor); }
	void Translate(float x, float y, float z);

	//const ModelAttributes& GetAttributes();
	Mat4<float> GetModelMatrix() const;
	std::vector<Mesh>& GetMeshes() { return m_meshes; }
	const ModelEdge& GetAdjacencyTable() { return *m_edges; } // To be able to access the data I am retrieving via []

private:
	Vec3<float> m_position{ 0, 0, 0 };
	Vec3<float> m_scale{1, 1, 1};
	Quaternion m_delta;
	std::vector<Mesh> m_meshes;
	std::shared_ptr<ModelEdge> m_edges; // Shared per same Model, only thing that changes is the mesh material, and the object specific model matrix.
	/*
	* 
	std::unordered_map<uint64_t, std::vector<uint32_t>> adjacencyTable; -> share between multiple meshes.
	std::vector<Vertex> processedMesh; -> each mesh has this
	uint64_t MakeEdgeKey(uint32_t a, uint32_t b);

	TriangleContext PreProcessMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
	*/
	//ModelAttributes attributes;
};