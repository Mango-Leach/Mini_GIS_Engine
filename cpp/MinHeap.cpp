#include "MinHeap.h"
#include <iostream>
#include <stdexcept>
#include <iomanip>

using namespace std;

// Restore min-heap property by bubbling element UP
// Called after insertion at the end of the array
void MinHeap::heapifyUp(int index) {
    while (index > 0 && heap[parent(index)].first > heap[index].first) {
        swap(heap[parent(index)], heap[index]);
        index = parent(index);
    }
}

// Restore min-heap property by sinking element DOWN
// Called after replacing root with last element during extractMin
void MinHeap::heapifyDown(int index) {
    int smallest = index;
    int left = leftChild(index);
    int right = rightChild(index);
    int n = (int)heap.size();

    if (left < n && heap[left].first < heap[smallest].first) {
        smallest = left;
    }
    if (right < n && heap[right].first < heap[smallest].first) {
        smallest = right;
    }

    if (smallest != index) {
        swap(heap[index], heap[smallest]);
        heapifyDown(smallest);
    }
}

// Insert a new (distance, point) pair into the heap
void MinHeap::insert(double distance, const Point& p) {
    heap.push_back(make_pair(distance, p));
    heapifyUp((int)heap.size() - 1);
}

// Remove and return the element with minimum distance
pair<double, Point> MinHeap::extractMin() {
    if (heap.empty()) {
        throw runtime_error("MinHeap::extractMin() called on empty heap");
    }

    pair<double, Point> minElement = heap[0];
    heap[0] = heap.back();
    heap.pop_back();

    if (!heap.empty()) {
        heapifyDown(0);
    }

    return minElement;
}

// View the minimum element without removing it
pair<double, Point> MinHeap::peek() const {
    if (heap.empty()) {
        throw runtime_error("MinHeap::peek() called on empty heap");
    }
    return heap[0];
}

// Display all elements in the heap (level-order)
void MinHeap::display() const {
    if (heap.empty()) {
        cout << "  (heap is empty)" << endl;
        return;
    }
    for (int i = 0; i < (int)heap.size(); i++) {
        cout << "  dist=" << fixed << setprecision(2) << heap[i].first
             << "  " << heap[i].second << endl;
    }
}
