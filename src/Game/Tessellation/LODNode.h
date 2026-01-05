#pragma once
#include <cstdint>
#include <unordered_map>
#include <string>

//struct HashKey;

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

