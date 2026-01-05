#pragma once
#include <cstdint>
#include <unordered_map>
#include <string>
#include "Math/Vec3.h"
#include <Graphics/Vertex.h>

struct MeshPosition {
	Vec3<float> position{};
};

struct TriangleContext { // Again temp idea for now just so we can easily access the stuff
	std::vector<uint8_t> depthLevels; // index of base vertex and provides the depth level 
	//std::unordered_map<std::pair<int, int>, int> edgeToTri; // only used for short time.
	//std::vector<uint32_t> adjacencyTable; // initialize to total mesh triangle count with value -1. Should point to neighbour base index that is neighbour of longest edge.
	std::unordered_map<uint64_t, std::vector<uint32_t>> adjacencyTable;
	std::vector<Vertex> processedMesh;
};

// This is simply for the first pass, so we only interpolate the position, and don't need to worry about the rest, while we see how deep we need to go in terms of splitting
struct TriangleNode { // This will hopefully be able to be used with both the base triangle node, as well as the split children of it.
	uint64_t nodeID; // Bitwise ID starts at 0.
	Vec3<float> v0, v1, v2; // These should be sorted, since CCW, v2->v0 should contain longest edge. (need to rotate vertices to ensure that). This is the Mesh Position
	uint32_t baseTriIdx; // 3 vertices, this v0: index * 3, v1: +1, v2: +2
	int depth;

	int64_t neighbourBaseTriangleIdx[3]; // -1 = invalid TriangleIdx. 0: v0->v1, 1: v1->v2, 2: v2->v0;

	//int longestEdgeIdx; // which edge is the longest edge.
	//uint32_t neighbourVertexIdx[3]; // 0 is v0->v1, 1: v1->v2, 2: v2->v0 // OR should it be the neighbours ID rather than the base neighbour idx?
};

uint64_t MakeEdgeKey(uint32_t a, uint32_t b);

TriangleContext PreProcessMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

/*
TessellationMesh PreProcessMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
	int triIdx;

	std::unordered_map<std::string, std::vector<uint32_t>> meshAdjacencyMap;
	std::vector<LODNode> triangleLODTree;
	std::vector<Vertex> taggedVertices;

	triangleLODTree.reserve(indices.size() / 3);

	for (int i = 0; i < indices.size(); i += 3) {
		triIdx = static_cast<int>(i / 3);
		//////////////////////////////////////////
		///									   ///
		///			Triangle LOD Tree		   ///
		///									   ///
		//////////////////////////////////////////


		triangleLODTree.push_back(LODNode());

		LODNode* node = &triangleLODTree.at(triIdx);
		node->m_vertexIndices[0] = i;
		node->m_vertexIndices[1] = i + 1;
		node->m_vertexIndices[2] = i + 2;

		//


		//////////////////////////////////////////
		///									   ///
		///			Adjacency Edge Map		   ///
		///									   ///
		//////////////////////////////////////////

		// Triangle pre-process.
		// need a way to tag the index it used, but anyways.
		Vertex v0 = vertices[indices[i]];
		Vertex v1 = vertices[indices[i + 1]];
		Vertex v2 = vertices[indices[i + 2]];

		v0.SetMeshIndex(indices[i]);
		v1.SetMeshIndex(indices[i + 1]);
		v2.SetMeshIndex(indices[i + 2]);

		v0.SetUniqueIndex(i);
		v1.SetUniqueIndex(i + 1);
		v2.SetUniqueIndex(i + 2);


		// edge1 v0 -> v1
		std::string edge;

		if (indices[i] < indices[i + 1]) {
			edge = std::to_string(indices[i]) + ":" + std::to_string(indices[i + 1]);
		}
		else {
			edge = std::to_string(indices[i + 1]) + ":" + std::to_string(indices[i]);
		}

		if (!meshAdjacencyMap[edge].empty()) {
			node->m_neighboursIndex[edge] = meshAdjacencyMap[edge][0];
			triangleLODTree.at(meshAdjacencyMap[edge][0]).m_neighboursIndex[edge] = triIdx;
		}
		meshAdjacencyMap[edge].push_back(triIdx);

		// edge2 v1 -> v2
		if (indices[i + 1] < indices[i + 2]) {
			edge = std::to_string(indices[i + 1]) + ":" + std::to_string(indices[i + 2]);
		}
		else {
			edge = std::to_string(indices[i + 2]) + ":" + std::to_string(indices[i + 1]);
		}

		if (!meshAdjacencyMap[edge].empty()) {
			node->m_neighboursIndex[edge] = meshAdjacencyMap[edge][0];
			triangleLODTree.at(meshAdjacencyMap[edge][0]).m_neighboursIndex[edge] = triIdx;
		}
		meshAdjacencyMap[edge].push_back(triIdx);

		// edge3 v2 -> v3
		if (indices[i + 2] < indices[i]) {
			edge = std::to_string(indices[i + 2]) + ":" + std::to_string(indices[i]);
		}
		else {
			edge = std::to_string(indices[i]) + ":" + std::to_string(indices[i + 2]);
		}

		if (!meshAdjacencyMap[edge].empty()) {
			node->m_neighboursIndex[edge] = meshAdjacencyMap[edge][0];
			triangleLODTree.at(meshAdjacencyMap[edge][0]).m_neighboursIndex[edge] = triIdx;
		}
		meshAdjacencyMap[edge].push_back(triIdx);

		//triangleLODTree.push_back(&node);

		// find longest edge, and add it to the LODNOde.
		float edge1 = (v1.GetPosition() - v0.GetPosition()).GetMagnitudeSquared();
		float edge2 = (v2.GetPosition() - v1.GetPosition()).GetMagnitudeSquared();
		float edge3 = (v0.GetPosition() - v2.GetPosition()).GetMagnitudeSquared();

		int longestEdgeIdx = -1;

		if (edge1 >= edge2 && edge1 >= edge3) {
			longestEdgeIdx = 0;
		}
		else if (edge2 > edge1 && edge2 >= edge3) {
			longestEdgeIdx = 1;
		}
		else if (edge3 > edge1 && edge3 > edge2) {
			longestEdgeIdx = 2;
		}

		node->m_splitEdgeIdx = longestEdgeIdx;
		taggedVertices.push_back(v0);
		taggedVertices.push_back(v1);
		taggedVertices.push_back(v2);
	}

	return TessellationMesh(meshAdjacencyMap, triangleLODTree, taggedVertices);
}
*/


//struct HashKey;
/*
// Probably want to use rather than raw pointers some form of smart pointers.
struct LODNode {
	uint32_t m_vertexIndices[3];
	LODNode* m_children[2];
	//LODNode* m_neighbours[3]; // Can't easily figure out which side it should be v0->v1->v2.
	//std::unordered_map<std::string, LODNode*> m_neighbours; // 3 neighbours, the key is the same as the edge ajacency map edge id.
	std::unordered_map<std::string, uint32_t> m_neighboursIndex; // 3 neighbours, the key is the same as the edge ajacency map edge id.

	int m_splitEdgeIdx;
	int m_depth;
	bool m_isLeaf;
	bool m_isVisible;

	LODNode() : m_children{ 0 }, m_depth(0), m_isLeaf(true), m_isVisible(true) {}
};

struct LODBaseNode {
	uint32_t m_vertexIndices[3];
	LODNode* m_children[2];
	//LODNode* m_neighbours[3]; // Can't easily figure out which side it should be v0->v1->v2.
	//std::unordered_map<std::string, LODNode*> m_neighbours; // 3 neighbours, the key is the same as the edge ajacency map edge id.
	std::unordered_map<std::string, uint32_t> m_neighboursIndex; // 3 neighbours, the key is the same as the edge ajacency map edge id.

	int m_splitEdgeIdx;
	int m_depth;
	bool m_isLeaf;
	bool m_isVisible;

	LODBaseNode() : m_children{ 0 }, m_depth(0), m_isLeaf(true), m_isVisible(true) {}
};
*/
