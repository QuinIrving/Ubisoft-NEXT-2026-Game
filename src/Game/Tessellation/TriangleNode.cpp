#include "TriangleNode.h"

/*
void TriangleNode::forceNeighbourSplit(TriangleContext& context, int32_t neighbourPoolIdx, int currentPriority) {
	TriangleNode& neighbour = context.nodePool[neighbourPoolIdx];

	// check if the neighbour even exists in our heap (if not then it's already been split)
	if (neighbour.heapIndex == -1) {
		return;
	}

	int higherPriority = priority + 1;
	context.updatePriority(neighbourPoolIdx, higherPriority);
}*/


void TriangleNode::SplitLongestEdge(TriangleContext& context) {
	// Split our node into 2 children nodes, with new vertex midpoint in the middle of edge v2->v0, ensure we properly rotate our childrens vertices, so their v2, v0 don't include the midpoint.
	// Should put both into work queue

	// nice interpolation function for a view vertex would be nice.
	//ViewVertex m = ViewVertex::EdgeMidpointInterpolate();

}

int32_t TriangleNode::SplitAndMatchNeighbour(TriangleContext& context, int32_t neighbourIdx) {
	TriangleNode& neighbour = context.nodePool[neighbourIdx];
	// SHould only put the non-matching node into work queue, other one is returned to be added to the stack.

	// Split, child 1 goes to re-use parent node, and goes back to work queue
	// Child 2 checks for free-nodes if none, then creates new node, and is returned to be added to the urgent stack
	// Don't forget to update neighbour references to us as well for all 3 edges.
}

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
	//context.depthLevels = std::vector<uint8_t>(indices.size() / 3, 0);
	context.processedMesh.reserve(indices.size());

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