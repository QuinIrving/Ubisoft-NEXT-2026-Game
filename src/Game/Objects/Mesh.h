#pragma once
#include <unordered_map>
#include <vector>
#include "Graphics/Vertex.h"
#include "Graphics/ModelAttributes.h"
#include <memory>

// This MeshGeometry is made once per mesh in the model, it should be a std::shared_ptr in mesh, as only the material should change
struct MeshGeometry {
	std::unordered_map<uint64_t, std::vector<uint32_t>> adjacencyTable;
	std::vector<Vertex> processedMesh;
};

struct Mesh {
public:
	// vertices (only generated once) -> shared after being pre-processed for our tessellation pipeline.
	std::shared_ptr<MeshGeometry> geometry; // If an object has already been loaded once, it simply copies from the base model, that will provide the base values, material can be updated though
	//Material material; // instance specific
};