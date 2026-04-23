#ifndef POINT_H
#define POINT_H

#include <string>
#include <iostream>
#include <cmath>

using namespace std;

// Represents a 2D geographic point with a label
struct Point {
    double x;      // X-coordinate (longitude / easting)
    double y;      // Y-coordinate (latitude / northing)
    string label;  // Human-readable name for the point

    // Default constructor
    Point() : x(0), y(0), label("") {}

    // Parameterized constructor
    Point(double x, double y, string label = "")
        : x(x), y(y), label(label) {}

    // Display the point
    void display() const {
        cout << "[" << label << "] (" << x << ", " << y << ")";
    }

    // Equality operator (compares coordinates, ignores label)
    bool operator==(const Point& other) const {
        return (fabs(x - other.x) < 1e-9) && (fabs(y - other.y) < 1e-9);
    }

    bool operator!=(const Point& other) const {
        return !(*this == other);
    }

    // Compare by x-coordinate (used by AVL Tree)
    bool operator<(const Point& other) const {
        if (fabs(x - other.x) < 1e-9)
            return y < other.y;
        return x < other.x;
    }

    bool operator>(const Point& other) const {
        return other < *this;
    }

    // Stream insertion operator
    friend ostream& operator<<(ostream& os, const Point& p) {
        os << "[" << p.label << "] (" << p.x << ", " << p.y << ")";
        return os;
    }
};

#endif // POINT_H
