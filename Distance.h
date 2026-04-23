#ifndef DISTANCE_H
#define DISTANCE_H

#include "Point.h"

// Distance calculator class — provides different distance metrics
// Used by QuadTree radius queries and KNN search
class Distance {
public:
    // Euclidean distance: sqrt((x2-x1)^2 + (y2-y1)^2)
    // Standard straight-line distance in 2D plane
    static double euclidean(const Point& a, const Point& b);

    // Manhattan distance: |x2-x1| + |y2-y1|
    // Grid-based distance (like city blocks)
    static double manhattan(const Point& a, const Point& b);

    // Squared Euclidean distance (avoids sqrt for faster comparisons)
    static double euclideanSquared(const Point& a, const Point& b);
};

#endif // DISTANCE_H
