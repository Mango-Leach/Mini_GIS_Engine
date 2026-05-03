#ifndef MINHEAP_H
#define MINHEAP_H

#include "Point.h"
#include <vector>
#include <utility>

// ============================================================================
// MinHeap — Priority Queue for K-Nearest Neighbor Search
// ============================================================================
// Stores (distance, Point) pairs ordered by distance (min at top).
// Used in KNN: after collecting candidate points from the QuadTree,
// we insert them into the MinHeap and extractMin K times to get
// the K closest points.
//
// Time Complexity:
//   Insert:      O(log n)
//   ExtractMin:  O(log n)
//   Peek:        O(1)
//   Build Heap:  O(n)
//
// Why MinHeap and not Fibonacci/Binomial Heap?
//   - We don't need decrease-key (no Dijkstra-like relaxation)
//   - Simple binary heap is sufficient and has lower constant factors
// ============================================================================
class MinHeap {
private:
    vector<pair<double, Point>> heap;  // (distance, point) pairs

    // Index calculations
    int parent(int i) const { return (i - 1) / 2; }
    int leftChild(int i) const { return 2 * i + 1; }
    int rightChild(int i) const { return 2 * i + 2; }

    // Restore heap property upward (after insert)
    void heapifyUp(int index);

    // Restore heap property downward (after extract)
    void heapifyDown(int index);

public:
    MinHeap() {}

    // Insert a (distance, point) pair
    void insert(double distance, const Point& p);

    // Remove and return the pair with minimum distance
    pair<double, Point> extractMin();

    // View the minimum without removing
    pair<double, Point> peek() const;

    // Number of elements in the heap
    int getSize() const { return (int)heap.size(); }

    // Check if heap is empty
    bool empty() const { return heap.empty(); }

    // Display heap contents (for debugging)
    void display() const;
};

#endif // MINHEAP_H
