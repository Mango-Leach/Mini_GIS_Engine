#ifndef QUADTREE_H
#define QUADTREE_H

#include "Point.h"
#include <vector>

// Axis-Aligned Bounding Box — defines a rectangular region in 2D space
// Used by QuadTree to represent node boundaries
struct AABB {
    double cx, cy;           // Center of the bounding box
    double halfWidth;        // Half-width (extends left and right from center)
    double halfHeight;       // Half-height (extends up and down from center)

    AABB();
    AABB(double cx, double cy, double halfWidth, double halfHeight);

    // Check if a point falls inside this bounding box
    bool contains(const Point& p) const;

    // Check if this bounding box intersects with another
    bool intersects(const AABB& other) const;
};

// ============================================================================
// QuadTree — Recursive spatial partitioning data structure
// ============================================================================
// Each node covers a rectangular region (AABB) and holds up to 'capacity'
// points. When the capacity is exceeded, the node subdivides into 4 children
// (NW, NE, SW, SE), each covering one quadrant of the parent's region.
//
// Time Complexity:
//   Insert:      O(log n) average, O(n) worst case (degenerate)
//   Range Query: O(n) worst case, but typically much better due to pruning
//   Space:       O(n)
//
// This is THE canonical data structure for 2D spatial indexing in GIS systems.
// ============================================================================
class QuadTree {
private:
    AABB boundary;             // Region this node covers
    vector<Point> points;      // Points stored in this node
    int capacity;              // Max points before subdivision

    bool divided;              // Whether this node has been subdivided
    QuadTree* NW;              // Northwest child
    QuadTree* NE;              // Northeast child
    QuadTree* SW;              // Southwest child
    QuadTree* SE;              // Southeast child

    // Split this node into 4 children
    void subdivide();

public:
    // Constructor: boundary region and max points per node
    QuadTree(AABB boundary, int capacity = 4);

    // Destructor: recursively free children
    ~QuadTree();

    // Insert a point into the QuadTree
    // Returns false if the point is outside the boundary
    bool insert(const Point& p);

    // Find all points within a rectangular range
    void queryRange(const AABB& range, vector<Point>& found) const;

    // Find all points within a given radius of a center point
    void queryRadius(const Point& center, double radius, vector<Point>& found) const;

    // Collect all points stored in the tree
    void getAllPoints(vector<Point>& allPoints) const;

    // Count total points in the tree
    int size() const;

    // Visualize the QuadTree structure (text-based)
    void display(int depth = 0) const;
};

#endif // QUADTREE_H
