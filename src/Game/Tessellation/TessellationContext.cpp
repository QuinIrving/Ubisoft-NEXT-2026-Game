#include "TessellationContext.h"

// Pre-process step If I can, each object it should be simple while I process. I also want to tag my vertices at some points as being a certain triangle index (probably via viewvert level)
// Need to store this somewhere, perhaps uniquely per new mesh.
// Have a mesh object, and we simply keep shared-ptrs of it, and then we can share the triangle tree, and adjacency map

 // takes in a pair of the lowest vertex id of an edge, and the other vertex id of the edge,
// and contains a list of triangle id's sharing the edge (should only be 1, but vector just in case)


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