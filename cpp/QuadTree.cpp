#include "QuadTree.h"
#include "Distance.h"
#include <iostream>
#include <iomanip>

using namespace std;

// ============================================================================
// AABB Implementation
// ============================================================================

AABB::AABB() : cx(0), cy(0), halfWidth(0), halfHeight(0) {}

AABB::AABB(double cx, double cy, double halfWidth, double halfHeight)
    : cx(cx), cy(cy), halfWidth(halfWidth), halfHeight(halfHeight) {}

// Check if point p lies within this bounding box (inclusive boundaries)
bool AABB::contains(const Point& p) const {
    return (p.x >= cx - halfWidth  && p.x <= cx + halfWidth &&
            p.y >= cy - halfHeight && p.y <= cy + halfHeight);
}

// Check if this AABB overlaps with another AABB
// Two boxes do NOT intersect if one is entirely to the left, right,
// above, or below the other
bool AABB::intersects(const AABB& other) const {
    return !(other.cx - other.halfWidth  > cx + halfWidth  ||
             other.cx + other.halfWidth  < cx - halfWidth  ||
             other.cy - other.halfHeight > cy + halfHeight ||
             other.cy + other.halfHeight < cy - halfHeight);
}

// ============================================================================
// QuadTree Implementation
// ============================================================================

QuadTree::QuadTree(AABB boundary, int capacity)
    : boundary(boundary), capacity(capacity), divided(false),
      NW(nullptr), NE(nullptr), SW(nullptr), SE(nullptr) {}

QuadTree::~QuadTree() {
    delete NW;
    delete NE;
    delete SW;
    delete SE;
}

// Subdivide this node into 4 quadrants
// Each child covers one quarter of the parent's area
//
//   NW  |  NE
//  -----+-----
//   SW  |  SE
//
void QuadTree::subdivide() {
    double x = boundary.cx;
    double y = boundary.cy;
    double hw = boundary.halfWidth / 2.0;
    double hh = boundary.halfHeight / 2.0;

    // Create four children, each covering one quadrant
    NW = new QuadTree(AABB(x - hw, y + hh, hw, hh), capacity);
    NE = new QuadTree(AABB(x + hw, y + hh, hw, hh), capacity);
    SW = new QuadTree(AABB(x - hw, y - hh, hw, hh), capacity);
    SE = new QuadTree(AABB(x + hw, y - hh, hw, hh), capacity);

    divided = true;
}

// Insert a point into the QuadTree
// If this node is full and not yet divided, subdivide first
// Then try to insert into the appropriate child
bool QuadTree::insert(const Point& p) {
    // Ignore points outside this node's boundary
    if (!boundary.contains(p)) {
        return false;
    }

    // If there's room in this node, store the point here
    if ((int)points.size() < capacity) {
        points.push_back(p);
        return true;
    }

    // Otherwise, subdivide if we haven't already
    if (!divided) {
        subdivide();
    }

    // Try to insert into one of the four children
    if (NW->insert(p)) return true;
    if (NE->insert(p)) return true;
    if (SW->insert(p)) return true;
    if (SE->insert(p)) return true;

    // Should never reach here if boundary.contains(p) is true
    return false;
}

// Range Query: find all points inside a rectangular AABB
// Prunes entire subtrees whose boundaries don't intersect the query range
void QuadTree::queryRange(const AABB& range, vector<Point>& found) const {
    // If this node's boundary doesn't intersect the range, skip entirely
    if (!boundary.intersects(range)) {
        return;
    }

    // Check points in this node
    for (const Point& p : points) {
        if (range.contains(p)) {
            found.push_back(p);
        }
    }

    // If subdivided, recurse into children
    if (divided) {
        NW->queryRange(range, found);
        NE->queryRange(range, found);
        SW->queryRange(range, found);
        SE->queryRange(range, found);
    }
}

// Radius Query: find all points within distance 'radius' of 'center'
// Uses AABB intersection for quick pruning, then exact distance check
void QuadTree::queryRadius(const Point& center, double radius,
                           vector<Point>& found) const {
    // Create a bounding box around the circle for fast pre-check
    AABB rangeBB(center.x, center.y, radius, radius);

    // If this node doesn't even intersect the bounding box, skip
    if (!boundary.intersects(rangeBB)) {
        return;
    }

    // Check each point in this node
    for (const Point& p : points) {
        if (Distance::euclidean(center, p) <= radius) {
            found.push_back(p);
        }
    }

    // Recurse into children
    if (divided) {
        NW->queryRadius(center, radius, found);
        NE->queryRadius(center, radius, found);
        SW->queryRadius(center, radius, found);
        SE->queryRadius(center, radius, found);
    }
}

// Collect every point stored in the tree
void QuadTree::getAllPoints(vector<Point>& allPoints) const {
    for (const Point& p : points) {
        allPoints.push_back(p);
    }
    if (divided) {
        NW->getAllPoints(allPoints);
        NE->getAllPoints(allPoints);
        SW->getAllPoints(allPoints);
        SE->getAllPoints(allPoints);
    }
}

// Count all points in the tree
int QuadTree::size() const {
    int count = (int)points.size();
    if (divided) {
        count += NW->size();
        count += NE->size();
        count += SW->size();
        count += SE->size();
    }
    return count;
}

// Display the QuadTree structure with indentation
void QuadTree::display(int depth) const {
    string indent(depth * 2, ' ');
    string region = "[" + to_string(boundary.cx - boundary.halfWidth) + ", " +
                    to_string(boundary.cy - boundary.halfHeight) + "] -> [" +
                    to_string(boundary.cx + boundary.halfWidth) + ", " +
                    to_string(boundary.cy + boundary.halfHeight) + "]";

    cout << indent << "Region: " << region
         << " | Points: " << points.size() << endl;

    // Show points at this node
    for (const Point& p : points) {
        cout << indent << "  -> " << p << endl;
    }

    // Recurse into children
    if (divided) {
        cout << indent << "  [NW]:" << endl;
        NW->display(depth + 1);
        cout << indent << "  [NE]:" << endl;
        NE->display(depth + 1);
        cout << indent << "  [SW]:" << endl;
        SW->display(depth + 1);
        cout << indent << "  [SE]:" << endl;
        SE->display(depth + 1);
    }
}
