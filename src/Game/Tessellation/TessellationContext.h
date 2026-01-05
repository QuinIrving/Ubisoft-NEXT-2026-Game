#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "Graphics/Vertex.h"
#include "LODNode.h"

// for the actual context
class TessellationContext {
	std::unordered_map<std::string, uint32_t> midpointCache; // just so once we calculate the midpoint once for an edge, the other triangle in the diamond can simply retrieve it instead
	// of recalculating
	float screenScale;
	float threshold; // LOD sesnitivity (should be capable by changing via a settings menu)
};


// Just temp to test out the idea and return types.
struct TessellationMesh {
	std::unordered_map<std::string, std::vector<uint32_t>> m_edgeAjacencyMap;
	std::vector<LODNode> m_triangleLODTree; // contains a list of the base triangles, each triangle then can reference it's split subtriangles through tessellation.
	std::vector<Vertex> m_taggedVertices;

	TessellationMesh(std::unordered_map<std::string, std::vector<uint32_t>> edgeAjacencyMap, std::vector<LODNode> triangleLODTree, std::vector<Vertex> taggedVertices) :
		m_edgeAjacencyMap(edgeAjacencyMap), m_triangleLODTree(triangleLODTree), m_taggedVertices(taggedVertices) {};
};

TessellationMesh PreProcessMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);