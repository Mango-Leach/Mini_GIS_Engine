// Distance.js — translated from Distance.h / Distance.cpp
// Static distance utility class

export class Distance {

    // Euclidean distance: sqrt((x2-x1)^2 + (y2-y1)^2)
    static euclidean(a, b) {
        const dx = a.x - b.x;
        const dy = a.y - b.y;
        return Math.sqrt(dx * dx + dy * dy);
    }

    // Manhattan distance: |x2-x1| + |y2-y1|
    static manhattan(a, b) {
        return Math.abs(a.x - b.x) + Math.abs(a.y - b.y);
    }

    // Squared Euclidean — avoids sqrt, use when only comparing distances
    static euclideanSquared(a, b) {
        const dx = a.x - b.x;
        const dy = a.y - b.y;
        return dx * dx + dy * dy;
    }
}