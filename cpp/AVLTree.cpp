#include "AVLTree.h"
#include <iostream>
#include <algorithm>

using namespace std;

// ============================================================================
// AVL Tree Implementation
// ============================================================================

AVLTree::AVLTree() : root(nullptr) {}

AVLTree::~AVLTree() {
    destroy(root);
}

void AVLTree::destroy(AVLNode* node) {
    if (node) {
        destroy(node->left);
        destroy(node->right);
        delete node;
    }
}

// --- Height & Balance ---

int AVLTree::height(AVLNode* node) const {
    return node ? node->height : 0;
}

int AVLTree::balanceFactor(AVLNode* node) const {
    return node ? height(node->left) - height(node->right) : 0;
}

void AVLTree::updateHeight(AVLNode* node) {
    if (node) {
        node->height = 1 + max(height(node->left), height(node->right));
    }
}

// --- Rotations ---

// Right Rotation: y becomes right child of x
// Before: y(x(A,B), C)  =>  After: x(A, y(B,C))
AVLNode* AVLTree::rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* B = x->right;

    x->right = y;
    y->left = B;

    updateHeight(y);
    updateHeight(x);

    return x;  // New root of this subtree
}

// Left Rotation: x becomes left child of y
// Before: x(A, y(B,C))  =>  After: y(x(A,B), C)
AVLNode* AVLTree::rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* B = y->left;

    y->left = x;
    x->right = B;

    updateHeight(x);
    updateHeight(y);

    return y;  // New root of this subtree
}

// Balance a node: apply appropriate rotation(s) if |BF| > 1
AVLNode* AVLTree::balance(AVLNode* node) {
    updateHeight(node);
    int bf = balanceFactor(node);

    // Left-heavy
    if (bf > 1) {
        // Left-Right case: first rotate left child left
        if (balanceFactor(node->left) < 0) {
            node->left = rotateLeft(node->left);
        }
        // Left-Left case (or after LR fix): rotate right
        return rotateRight(node);
    }

    // Right-heavy
    if (bf < -1) {
        // Right-Left case: first rotate right child right
        if (balanceFactor(node->right) > 0) {
            node->right = rotateRight(node->right);
        }
        // Right-Right case (or after RL fix): rotate left
        return rotateLeft(node);
    }

    return node;  // Already balanced
}

// --- Insert ---

AVLNode* AVLTree::insert(AVLNode* node, const Point& p) {
    if (!node) {
        return new AVLNode(p);
    }

    if (p < node->data) {
        node->left = insert(node->left, p);
    } else if (p > node->data) {
        node->right = insert(node->right, p);
    } else {
        // Duplicate coordinates — update label if different
        node->data.label = p.label;
        return node;
    }

    return balance(node);
}

void AVLTree::insert(const Point& p) {
    root = insert(root, p);
}

// --- Remove ---

AVLNode* AVLTree::findMin(AVLNode* node) const {
    while (node && node->left) {
        node = node->left;
    }
    return node;
}

AVLNode* AVLTree::remove(AVLNode* node, const string& label, bool& found) {
    if (!node) return nullptr;

    // Search by label (must check all nodes since label isn't the sort key)
    if (node->data.label == label) {
        found = true;

        // Node with one child or no child
        if (!node->left || !node->right) {
            AVLNode* temp = node->left ? node->left : node->right;
            delete node;
            return temp;
        }

        // Node with two children: get inorder successor
        AVLNode* successor = findMin(node->right);
        node->data = successor->data;
        // Delete the successor by its label
        bool tempFound = false;
        node->right = remove(node->right, successor->data.label, tempFound);
    } else {
        // We must search both subtrees since label isn't the BST key
        node->left = remove(node->left, label, found);
        if (!found) {
            node->right = remove(node->right, label, found);
        }
    }

    if (!node) return nullptr;
    return balance(node);
}

bool AVLTree::remove(const string& label) {
    bool found = false;
    root = remove(root, label, found);
    return found;
}

// --- Traversal & Search ---

void AVLTree::inorder(AVLNode* node, vector<Point>& result) const {
    if (!node) return;
    inorder(node->left, result);
    result.push_back(node->data);
    inorder(node->right, result);
}

vector<Point> AVLTree::inorder() const {
    vector<Point> result;
    inorder(root, result);
    return result;
}

// 1D Range Search: find all points with xMin <= x <= xMax
// Prunes subtrees that can't contain matching x values
void AVLTree::rangeSearch(AVLNode* node, double xMin, double xMax,
                          vector<Point>& result) const {
    if (!node) return;

    // If current node's x is >= xMin, left subtree might have matches
    if (node->data.x >= xMin) {
        rangeSearch(node->left, xMin, xMax, result);
    }

    // Check current node
    if (node->data.x >= xMin && node->data.x <= xMax) {
        result.push_back(node->data);
    }

    // If current node's x is <= xMax, right subtree might have matches
    if (node->data.x <= xMax) {
        rangeSearch(node->right, xMin, xMax, result);
    }
}

vector<Point> AVLTree::rangeSearch(double xMin, double xMax) const {
    vector<Point> result;
    rangeSearch(root, xMin, xMax, result);
    return result;
}

// Find a point by its label
AVLNode* AVLTree::findByLabel(AVLNode* node, const string& label) const {
    if (!node) return nullptr;
    if (node->data.label == label) return node;

    AVLNode* found = findByLabel(node->left, label);
    if (found) return found;
    return findByLabel(node->right, label);
}

bool AVLTree::findByLabel(const string& label, Point& result) const {
    AVLNode* node = findByLabel(root, label);
    if (node) {
        result = node->data;
        return true;
    }
    return false;
}

bool AVLTree::isEmpty() const {
    return root == nullptr;
}

// --- Display (tree structure) ---

void AVLTree::display(AVLNode* node, string prefix, bool isLeft) const {
    if (!node) return;

    cout << prefix;
    cout << (isLeft ? "|-- " : "\\-- ");
    cout << node->data << " (h=" << node->height
         << ", bf=" << balanceFactor(node) << ")" << endl;

    display(node->left,  prefix + (isLeft ? "|   " : "    "), true);
    display(node->right, prefix + (isLeft ? "|   " : "    "), false);
}

void AVLTree::display() const {
    if (!root) {
        cout << "  (tree is empty)" << endl;
        return;
    }
    display(root, "", false);
}
