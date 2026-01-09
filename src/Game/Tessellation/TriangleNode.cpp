#include "TriangleNode.h"


/*
void TriangleNode::LeftChildSetup(TriangleNode& child, int32_t poolIndex) {
	child.v0 = v2;
	child.v2 = v1;

	// All neighbours prior should already be at -1 for both this free node, and the neighbours who use to reference it, so we can treat it like a fresh new node
	child.neighbours[0] = neighbours[2];
	child.neighbours[1] = poolIndex; // = our right neighbour a.k.a our parent
	child.neighbours[2] = neighbours[1];

	child.nodeID = nodeID << 1 | 0; // TODO: FIX
}

void TriangleNode::RightChildSetup(int32_t leftIndex) {
	v2 = v0;
	v0 = v1;

	
	neighbours[1] = neighbours[2];
	neighbours[2] = neighbours[0];
	neighbours[0] = leftIndex; // Left child

	nodeID = nodeID << 1 | 1; // TODO: FIX
}

void TriangleNode::UpdateLeftNeighbours(TriangleContext& context, TriangleNode& child, int32_t childIdx, int32_t poolIndex) {
	for (int n = 0; n < 3; ++n) {
		if (child.neighbours[n] != -1 && child.neighbours[n] != poolIndex) {
			TriangleNode& neighbour = context.nodePool[child.neighbours[n]];

			for (int i = 0; i < 3; ++i) {
				// if our neighbour was pointing to our parent, make it point to the new child, I guess this actually only matters for left-child... as our right child uses our parent..
				if (neighbour.neighbours[i] == poolIndex) {
					neighbour.neighbours[i] = childIdx;
				}
			}
		}
	}
}

void TriangleNode::GenericChildSetup(TriangleNode& child, ViewVertex m) {
	child.v1 = m;
	child.isCulled = false;
	child.baseTriIdx = baseTriIdx;
	child.depth = depth + 1;
}*/


// Can really only be used when we do a single child splitting, with both being pushed to the work queue
void TriangleNode::SingleSplitLongestEdge(TriangleContext& context, int32_t poolIdx) {
	// Split our node into 2 children nodes, with new vertex midpoint in the middle of edge v2->v0, ensure we properly rotate our childrens vertices, so their v2, v0 don't include the midpoint.
	// Should put both into work queue
	/*ViewVertex m = ViewVertex::EdgeMidpointInterpolate(v0, v2);
	int32_t leftChildIdx = -1;

	// Create the left child (1)
	if (!context.freeNodeIndices.empty()) {
		// child 1 uses this empty node,
		int32_t freeIdx = context.freeNodeIndices.back();
		context.freeNodeIndices.pop_back();

		TriangleNode& child1 = context.nodePool[freeIdx];
		this->LeftChildSetup(child1, poolIdx);
		this->GenericChildSetup(child1, m);
		this->UpdateLeftNeighbours(context, child1, leftChildIdx, poolIdx);
		
		leftChildIdx = freeIdx;
	}
	else {
		// child 1 creates a new node.
		leftChildIdx = context.nodePool.size();
		context.nodePool.push_back({});

		TriangleNode& child1 = context.nodePool[leftChildIdx];
		this->LeftChildSetup(child1, poolIdx);
		this->GenericChildSetup(child1, m);
		this->UpdateLeftNeighbours(context, child1, leftChildIdx, poolIdx);
	}

	// Create the right child (2)
	this->RightChildSetup(leftChildIdx);
	this->GenericChildSetup(*this, m);

	context.workQueue.push_back({ leftChildIdx, context.nodePool[leftChildIdx].nodeID});
	context.workQueue.push_back({ poolIdx, nodeID });*/
}

// stack is not empty, so we have a dependency waiting for one of our 2 childs (specifically the one matching the other's edge.
// TODO: Split this node and add the 1 child to the stack that is the same longest edge as whats currently on the back
//WorkItem dependentChild = TriangleNode::SingleSplitAndMatchNeighbour(context, work.nodeIdx, context.urgentStack.back().nodeIdx);
//context.urgentStack.push_back(dependentChild);
void TriangleNode::SingleSplitAndMatchNeighbour(TriangleContext& context, int32_t poolIdx, int32_t dependentNeighbourIdx) {
	int32_t leftChildIndex = TriangleNode::GetLeftChildIndex(context);
	TriangleNode& node = context.nodePool[poolIdx];

	TriangleNode& leftChild = context.nodePool[leftChildIndex];
	TriangleNode::GeneralChildrenSetup(context, node, leftChild);

	leftChild.neighbours[0] = node.neighbours[2];
	leftChild.neighbours[1] = poolIdx;
	leftChild.neighbours[2] = node.neighbours[1];

	TriangleNode& rightChild = node; // As our right child takes over our parent.
	rightChild.neighbours[1] = rightChild.neighbours[2];
	rightChild.neighbours[2] = rightChild.neighbours[0];
	rightChild.neighbours[0] = leftChildIndex; // Left child

	// Only need to update my left child neighbours to point to it, as neighbours for right child point to the parent, which is the right child.
	// Can also use this time to determine where to push a node
	bool isDependentOnLeftChild = false;
	for (int n = 0; n < 3; ++n) {
		int32_t neighbourIdx = leftChild.neighbours[n];
		if (neighbourIdx != -1 && neighbourIdx != poolIdx) {
			TriangleNode& neighbour = context.nodePool[neighbourIdx];

			if (neighbourIdx == dependentNeighbourIdx) {
				isDependentOnLeftChild = true;
			}

			for (int i = 0; i < 3; ++i) {
				// if our neighbour was pointing to our parent, make it point to the new child, I guess this actually only matters for left-child... as our right child uses our parent..
				if (neighbour.neighbours[i] == poolIdx) {
					neighbour.neighbours[i] = leftChildIndex;
				}
			}
		}
	}

	// This means the stack is empty, and no dependencies waiting on the child of this split so simply push all to the work queue
	if (dependentNeighbourIdx == -1) {
		context.workQueue.push_back({ leftChildIndex, leftChild.baseTriIdx, leftChild.nodeID });
		context.workQueue.push_back({ poolIdx, rightChild.baseTriIdx, rightChild.nodeID });
		return;
	} 

	// elsewise, they we found the side of the triangle that should be added to the stack
	if (isDependentOnLeftChild) {
		context.urgentStack.push_back({ leftChildIndex, leftChild.baseTriIdx, leftChild.nodeID });
		context.workQueue.push_back({ poolIdx, rightChild.baseTriIdx, rightChild.nodeID });
	}
	else {
		context.workQueue.push_back({ leftChildIndex, leftChild.baseTriIdx, leftChild.nodeID });
		context.urgentStack.push_back({ poolIdx, rightChild.baseTriIdx, rightChild.nodeID });
	}
	
	// Errors may occur due to winding order when I create children here, but I believe it should still be okay? Need to figure out the best for the neighbours and stuff

	// find the child that contains the neighbour idx after split, from that we provide the childs idx, and the nodeId as the work item back to be added to the queue
	// we will be the ones pushing it into the urgentStack
}



// The dependent idx will have a child that contains the neighbour idx, if the stack is empty, it means we don't need to worry about returning anything
void TriangleNode::DiamondSplit(TriangleContext& context, int32_t nodeIdx, int32_t dependentIdx, int32_t dependentNeighbourIdx) {
	int32_t neighbourLeftChildIdx = TriangleNode::GetLeftChildIndex(context);
	int32_t dependentLeftChildIdx = TriangleNode::GetLeftChildIndex(context);

	TriangleNode& neighbour = context.nodePool[nodeIdx];
	TriangleNode& dependent = context.nodePool[dependentIdx]; // the one with the same longest edge neighbour

	/**************************************************************/
	// 
	//						NEIGHBOUR HANDLING
	// 
	/**************************************************************/
	TriangleNode& neighbourLeftChild = context.nodePool[neighbourLeftChildIdx];
	TriangleNode::GeneralChildrenSetup(context, neighbour, neighbourLeftChild);
	TriangleNode& neighbourRightChild = neighbour;

	/**************************************************************/
	// 
	//						Dependent HANDLING
	// 
	/**************************************************************/
	TriangleNode& dependentLeftChild = context.nodePool[dependentLeftChildIdx];
	TriangleNode::GeneralChildrenSetup(context, dependent, dependentLeftChild);
	TriangleNode& dependentRightChild = dependent;
	
	/**************************************************************/
	// 
	//						Node Neighbours HANDLING
	// 
	/**************************************************************/
	neighbourLeftChild.neighbours[0] = dependentIdx;
	neighbourLeftChild.neighbours[1] = nodeIdx; // nodeIdx is our neighbourIdx... I know
	neighbourLeftChild.neighbours[2] = neighbour.neighbours[1];

	neighbourRightChild.neighbours[2] = neighbourRightChild.neighbours[0];
	neighbourRightChild.neighbours[0] = neighbourLeftChildIdx;
	neighbourRightChild.neighbours[1] = dependentLeftChildIdx;

	dependentLeftChild.neighbours[0] = nodeIdx;
	dependentLeftChild.neighbours[1] = dependentIdx;
	dependentLeftChild.neighbours[2] = dependent.neighbours[1];

	dependentRightChild.neighbours[2] = dependentRightChild.neighbours[0];
	dependentRightChild.neighbours[0] = dependentLeftChildIdx;
	dependentRightChild.neighbours[1] = neighbourLeftChildIdx;

	// Only have each child's 2nd neighbour to worry about checking
	int32_t nIdx = neighbourLeftChild.neighbours[2];
	TriangleNode::UpdateNeighbourNode(context, nodeIdx, neighbourLeftChildIdx, neighbourLeftChild.neighbours[2], -1);
	bool isDependentOnLeftChild = TriangleNode::UpdateNeighbourNode(context, dependentIdx, dependentLeftChildIdx, dependentLeftChild.neighbours[2], dependentNeighbourIdx);
	
	// stack pushing
	context.workQueue.push_back({ neighbourLeftChildIdx, neighbourLeftChild.baseTriIdx, neighbourLeftChild.nodeID });
	context.workQueue.push_back({ nodeIdx, neighbourRightChild.baseTriIdx, neighbourRightChild.nodeID });

	WorkItem depLeftChild = WorkItem(dependentLeftChildIdx, dependentLeftChild.baseTriIdx, dependentLeftChild.nodeID);
	WorkItem depRightChild = WorkItem(dependentIdx, dependentRightChild.baseTriIdx, dependentRightChild.nodeID);

	if (dependentNeighbourIdx == -1) {
		context.workQueue.push_back(depLeftChild);
		context.workQueue.push_back(depRightChild);
		return;
	}

	if (isDependentOnLeftChild) {
		context.workQueue.push_back(depRightChild);
		context.urgentStack.push_back(depLeftChild);
		return;
	} 

	context.workQueue.push_back(depLeftChild);
	context.urgentStack.push_back(depRightChild);
}

int32_t TriangleNode::GetLeftChildIndex(TriangleContext& context) {
	int32_t childIndex;
	if (!context.freeNodeIndices.empty()) {
		childIndex = context.freeNodeIndices.back();
		context.freeNodeIndices.pop_back();
	}
	else {
		childIndex = context.nodePool.size(); // Dangerous, can invalidate our references to a TriangleNode
		context.nodePool.push_back({});
	}

	return childIndex;
}

// In this case the parent gets transformed into the right child. In all except for the neighbours situation.
void TriangleNode::GeneralChildrenSetup(TriangleContext& context, TriangleNode& parent, TriangleNode& leftChild) {
	ViewVertex m = ViewVertex::EdgeMidpointInterpolate(parent.v0, parent.v2);
	leftChild.v0 = parent.v2;
	leftChild.v1 = ViewVertex(m);
	leftChild.v2 = parent.v1;

	leftChild.isCulled = false;
	leftChild.nodeID = parent.nodeID << 1 | 0;
	leftChild.baseTriIdx = parent.baseTriIdx;
	leftChild.depth = parent.depth + 1;

	TriangleNode& rightChild = parent; // parent is our right child.
	rightChild.v2 = rightChild.v0;
	rightChild.v0 = rightChild.v1;
	rightChild.v1 = ViewVertex(m);

	rightChild.nodeID = rightChild.nodeID << 1 | 1;
	rightChild.depth = rightChild.depth + 1;
}

bool TriangleNode::UpdateNeighbourNode(TriangleContext& context, int32_t parentIndex, int32_t leftChildIndex, int32_t neighbourIndex, int32_t dependentNeighbourIndex) {
	bool isDependentOnLeftChild = false;

	if (neighbourIndex != -1) {
		TriangleNode& newEdgeNeighbour = context.nodePool[neighbourIndex];

		if (neighbourIndex == dependentNeighbourIndex) {
			isDependentOnLeftChild = true;
		}

		for (int i = 0; i < 3; ++i) {
			// if our neighbour was pointing to our parent, make it point to the new child, I guess this actually only matters for left-child... as our right child uses our parent..
			if (newEdgeNeighbour.neighbours[i] == parentIndex) {
				newEdgeNeighbour.neighbours[i] = leftChildIndex;
			}
		}

	}

	return isDependentOnLeftChild;
}

uint64_t MakeEdgeKey(uint32_t a, uint32_t b) {
	if (b < a) {
		std::swap(a, b);
	}

	uint64_t key = (static_cast<uint64_t>(a) << 32) | b;
	return key;
}

/*
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
			* /
			rv0 = v1;
			rv1 = v2;
			rv2 = v0;
		}
		else if (longestEdgeIdx == 1) {
			// Must do 1 rotation to get the longest edge to be v2->v0
			/*
			v1->v2 is currently the longest. So: v2 maps to v0, v1 maps to v2, v0 maps to v1.
			* / 
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
}*/