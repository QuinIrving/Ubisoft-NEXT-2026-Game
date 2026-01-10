#pragma once
#include <unordered_map>
#include <vector>
#include "Graphics/Vertex.h"
#include "Graphics/Material.h"
#include <memory>

// This MeshGeometry is made once per mesh in the model, it should be a std::shared_ptr in mesh, as only the material should change
struct MeshGeometry {
	std::vector<Vertex> processedMesh;

	MeshGeometry(std::vector<Vertex> mesh) : processedMesh(mesh) {}
};

struct Mesh {
public:
	// vertices (only generated once) -> shared after being pre-processed for our tessellation pipeline.
	std::shared_ptr<MeshGeometry> geometry; // If an object has already been loaded once, it simply copies from the base model, that will provide the base values, material can be updated though
	Material material; // instance specific -> might also want to make it a shared ptr to a material, as we have them stored in a hashmap as well.
};