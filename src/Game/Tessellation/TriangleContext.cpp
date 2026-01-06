#include "TriangleContext.h"
#include "TriangleNode.h"

bool TriangleContext::HaveSameLongestEdge(TriangleNode& node1, TriangleNode& node2) {
	if (node1.v0.GetViewPosition() == node2.v0.GetViewPosition() && node1.v2.GetViewPosition() == node2.v2.GetViewPosition()) {
		return true;
	}
	else if (node1.v0.GetViewPosition() == node2.v2.GetViewPosition() && node1.v2.GetViewPosition() == node2.v0.GetViewPosition()) {
		return true;
	}

	return false;
}

/*
// This should probably all be in a nice heap class instead.
void TriangleContext::swapNodes(int32_t i0, int32_t i1) {
	// swap the nodePool indices in our heap
	std::swap(heap[i0], heap[i1]);

	// And update our nodes to know where they are in the heap now.
	nodePool[heap[i0]].heapIndex = i0;
	nodePool[heap[i1]].heapIndex = i1;
}

void TriangleContext::siftUp(int32_t idx) {
	while (idx > 0) {
		int32_t parent = (idx - 1) / 2;
		if (nodePool[heap[idx]].priority <= nodePool[heap[parent]].priority) {
			break;
		}

		swapNodes(idx, parent);
		idx = parent;
	}
}

void TriangleContext::siftDown(int32_t idx) {
	int32_t heapSize = static_cast<int32_t>(heap.size());
	int32_t left;
	int32_t right;

	while (idx < heapSize - 1) {
		int32_t largest = idx;
		left = 2 * idx + 1;
		right = 2 * idx + 2;


		if (left < heapSize && nodePool[heap[left]].priority > nodePool[heap[largest]].priority) {
			largest = left;
		}
		if (right < heapSize && nodePool[heap[right]].priority > nodePool[heap[largest]].priority) {
			largest = right;
		}

		if (largest == idx) {
			break;
		}

		swapNodes(idx, largest);
		idx = largest;
	}
}

void TriangleContext::removeNode(int32_t poolIdx) {
	int32_t heapIdx = nodePool[poolIdx].heapIndex;

	// Check if node is not in heap.
	if (heapIdx == -1) {
		return;
	}


	swapNodes(heapIdx, (int32_t)heap.size() - 1);

	nodePool[heap.back()].heapIndex = -1;
	heap.pop_back();

	if (heapIdx < heap.size()) {
		siftUp(heapIdx);
		siftDown(heapIdx);
	}
}

void TriangleContext::updatePriority(int32_t poolIdx, uint8_t newPriority) {
	uint8_t oldPriority = nodePool[poolIdx].priority;
	nodePool[poolIdx].priority = newPriority;

	if (newPriority == oldPriority) {
		return;
	}

	int32_t currentHeapPos = nodePool[poolIdx].heapIndex;

	if (newPriority > oldPriority) {
		siftUp(currentHeapPos);
	}
	else {
		siftDown(currentHeapPos);
	}
}

void TriangleContext::pushNode(int32_t poolIdx) {
	int32_t heapIdx = static_cast<int32_t>(heap.size());
	nodePool[poolIdx].heapIndex = heapIdx;

	heap.push_back(poolIdx);
	siftUp(heapIdx);
}

bool TriangleContext::containsNode(int32_t poolIdx) {
	if (poolIdx < 0 || poolIdx >= static_cast<int32_t>(nodePool.size())) {
		return false;
	}

	return nodePool[poolIdx].heapIndex != -1; // -1 indicates it's already been split / is a dead node, but because we re-use parents nodes for children, this shouldn't ever be the case.
}*/