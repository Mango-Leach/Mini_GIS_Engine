#ifndef AVLTREE_H
#define AVLTREE_H

#include "Point.h"
#include <vector>

// ============================================================================
// AVL Tree — Self-Balancing Binary Search Tree for Sorted Point Index
// ============================================================================
// Stores Points sorted by x-coordinate (ties broken by y-coordinate).
// Guarantees O(log n) insert, delete, and search by maintaining the
// balance factor (|height(left) - height(right)| <= 1) at every node.
//
// Used in the GIS Engine for:
//   1. Displaying all points in sorted order (inorder traversal)
//   2. 1D range search by x-coordinate
//   3. Finding a point by label for deletion
//
// Rotations:
//   - Left Rotation (LL imbalance)
//   - Right Rotation (RR imbalance)
//   - Left-Right Rotation (LR imbalance)
//   - Right-Left Rotation (RL imbalance)
// ============================================================================

// AVL Tree Node
struct AVLNode {
    Point data;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(const Point& p)
        : data(p), left(nullptr), right(nullptr), height(1) {}
};

class AVLTree {
private:
    AVLNode* root;

    // --- Internal helper functions ---
    int height(AVLNode* node) const;
    int balanceFactor(AVLNode* node) const;
    void updateHeight(AVLNode* node);

    // Rotations
    AVLNode* rotateRight(AVLNode* y);
    AVLNode* rotateLeft(AVLNode* x);

    // Balance a node after insertion/deletion
    AVLNode* balance(AVLNode* node);

    // Recursive insert
    AVLNode* insert(AVLNode* node, const Point& p);

    // Recursive delete
    AVLNode* remove(AVLNode* node, const string& label, bool& found);

    // Find the node with minimum value (leftmost)
    AVLNode* findMin(AVLNode* node) const;

    // Inorder traversal (collects points sorted by x)
    void inorder(AVLNode* node, vector<Point>& result) const;

    // 1D range search by x-coordinate
    void rangeSearch(AVLNode* node, double xMin, double xMax,
                     vector<Point>& result) const;

    // Find a point by label
    AVLNode* findByLabel(AVLNode* node, const string& label) const;

    // Recursive destructor
    void destroy(AVLNode* node);

    // Display tree structure
    void display(AVLNode* node, string prefix, bool isLeft) const;

public:
    AVLTree();
    ~AVLTree();

    // Public interface
    void insert(const Point& p);
    bool remove(const string& label);
    vector<Point> inorder() const;
    vector<Point> rangeSearch(double xMin, double xMax) const;
    bool findByLabel(const string& label, Point& result) const;
    bool isEmpty() const;
    void display() const;
};

#endif // AVLTREE_H
