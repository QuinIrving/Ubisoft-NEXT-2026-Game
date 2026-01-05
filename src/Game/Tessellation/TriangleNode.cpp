#include "TriangleNode.h"

uint64_t MakeEdgeKey(uint32_t a, uint32_t b) {
	if (b < a) {
		std::swap(a, b);
	}

	uint64_t key = (static_cast<uint64_t>(a) << 32) | b;
	return key;
}

TriangleContext PreProcessMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
	// Take in a mesh in model space, and want to pre-determine the general neighbour structure, and re-arrange the vertices as necessary to fit it via rotation

	TriangleContext context;
	context.depthLevels = std::vector<uint8_t>(indices.size() / 3, 0);
	//context.adjacencyTable = std::vector<uint32_t>(indices.size() / 3, -1);
	context.processedMesh.reserve(indices.size());

	// 2 mesh indices of the edges for a triangle, and pass in it's triangle index for storing once neighbour is found
	//std::unordered_map<uint64_t, std::vector<uint32_t>> allEdgesToTris;

	// Pre-process the triangles I guess;
	for (int i = 0; i < indices.size(); i += 3) {
		int triIdx = static_cast<int>(i / 3);

		uint32_t i0 = indices[i];
		uint32_t i1 = indices[i + 1];
		uint32_t i2 = indices[i + 2];

		uint64_t key1 = MakeEdgeKey(i0, i1);
		uint64_t key2 = MakeEdgeKey(i1, i2);
		uint64_t key3 = MakeEdgeKey(i0, i2);

		context.adjacencyTable[key1].push_back(triIdx);
		context.adjacencyTable[key2].push_back(triIdx);
		context.adjacencyTable[key3].push_back(triIdx);
	}

	for (int i = 0; i < indices.size(); i += 3) {
		int triIdx = static_cast<int>(i / 3);

		uint32_t i0 = indices[i];
		uint32_t i1 = indices[i + 1];
		uint32_t i2 = indices[i + 2];

		// triangles shouldn't have the same vertex twice.
		if (i0 == i1 || i1 == i2 || i2 == i0) {
			throw std::runtime_error("Multiple duplicate vertices on one triangle, leading to a degenerate triangle in pre-processing.");
		}

		Vertex v0 = vertices[i0];
		Vertex v1 = vertices[i1];
		Vertex v2 = vertices[i2];
		v0.SetMeshIndex(i0);
		v1.SetMeshIndex(i1);
		v2.SetMeshIndex(i2);

		Vertex rv0, rv1, rv2; // Our post-rotation vertices.


		int longestEdgeIdx; // 0: v0->v1, 1: v1->v2, 2: v2->v0

		float edge1 = (v1.GetPosition() - v0.GetPosition()).GetMagnitudeSquared();
		float edge2 = (v2.GetPosition() - v1.GetPosition()).GetMagnitudeSquared();
		float edge3 = (v0.GetPosition() - v2.GetPosition()).GetMagnitudeSquared();

		if (edge1 >= edge2 && edge1 >= edge3) {
			longestEdgeIdx = 0;
		}
		else if (edge2 >= edge1 && edge2 >= edge3) {
			longestEdgeIdx = 1;
		}
		else if (edge3 >= edge1 && edge3 >= edge2) {
			longestEdgeIdx = 2;
		}
		else {
			longestEdgeIdx = -1;
		}

		// CCW rotation. v1 obtains v0, v2 obtains v1, v0 obtains v2.
		if (longestEdgeIdx == 0) {
			// Must do 2 rotations to get the longest edge to be v2->v0
			/*
			v0->v1 is currently the longest, so v1 maps to v0, v0 maps to v2, and v2 maps to v1
			*/
			rv0 = v1;
			rv1 = v2;
			rv2 = v0;
		}
		else if (longestEdgeIdx == 1) {
			// Must do 1 rotation to get the longest edge to be v2->v0
			/*
			v1->v2 is currently the longest. So: v2 maps to v0, v1 maps to v2, v0 maps to v1.
			*/
			rv0 = v2;
			rv1 = v0;
			rv2 = v1;
		}
		else if (longestEdgeIdx == 2) {
			rv0 = v0;
			rv1 = v1;
			rv2 = v2;
		}
		else if (longestEdgeIdx == -1) {
			// something went wrong throw an error.
			throw std::runtime_error("Longest Edge somehow was -1\n");
		}


		context.processedMesh.insert(context.processedMesh.end(), { rv0, rv1, rv2 });
	}

	return context;
}