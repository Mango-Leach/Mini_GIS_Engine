#include "Distance.h"
#include <cmath>

// Euclidean distance: sqrt((x2-x1)^2 + (y2-y1)^2)
// Time Complexity: O(1)
double Distance::euclidean(const Point& a, const Point& b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return sqrt(dx * dx + dy * dy);
}

// Manhattan distance: |x2-x1| + |y2-y1|
// Time Complexity: O(1)
double Distance::manhattan(const Point& a, const Point& b) {
    return fabs(a.x - b.x) + fabs(a.y - b.y);
}

// Squared Euclidean distance — avoids costly sqrt()
// Useful when we only need to COMPARE distances (ordering is preserved)
// Time Complexity: O(1)
double Distance::euclideanSquared(const Point& a, const Point& b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return dx * dx + dy * dy;
}
