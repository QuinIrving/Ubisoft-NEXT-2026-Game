#pragma once
#include <vector>
#include <unordered_map>
#include "Graphics/Vertex.h"
#include <deque>

struct TriangleNode;

struct WorkItem {
	int32_t nodeIdx;
	uint32_t baseTriIdx;
	uint64_t nodeId; // Allows for up to 64 depth, probably much more than we need.
	WorkItem(int32_t nodeIdx, uint32_t baseTriIdx, uint64_t nodeID) : nodeIdx(nodeIdx), baseTriIdx(baseTriIdx), nodeId(nodeID) {}
};

struct TriangleContext { // Again temp idea for now just so we can easily access the stuff
	//std::vector<uint8_t> depthLevels; // index of base vertex and provides the depth level 
	//std::unordered_map<std::pair<int, int>, int> edgeToTri; // only used for short time.
	//std::vector<uint32_t> adjacencyTable; // initialize to total mesh triangle count with value -1. Should point to neighbour base index that is neighbour of longest edge.
	std::unordered_map<uint64_t, std::vector<uint32_t>> adjacencyTable;
	std::vector<Vertex> processedMesh;

	std::vector<TriangleNode> nodePool;
	std::deque<WorkItem> workQueue;
	std::vector<WorkItem> urgentStack; // for once we need to go down a chain of dependencies of different longest-edge neighbours
	std::vector<int32_t> freeNodeIndices; // For nodes that were culled, and can re-used by child nodes on split instead of creating cold new nodes.


	static bool HaveSameLongestEdge(TriangleNode& node1, TriangleNode& node2);
	/*std::vector<int32_t> heap;

	// This should probably all be in a nice heap class instead.
	void swapNodes(int32_t i0, int32_t i1);
	void siftUp(int32_t idx);
	void siftDown(int32_t idx);
	void removeNode(int32_t poolIdx);
	void updatePriority(int32_t poolIdx, uint8_t newPriority);
	void pushNode(int32_t poolIdx);
	bool containsNode(int32_t poolIdx);*/
	// pop
};