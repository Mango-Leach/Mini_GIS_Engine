#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cmath>
#include <limits>

#ifdef _WIN32
#include <windows.h>
#endif

#include "Point.h"
#include "Distance.h"
#include "QuadTree.h"
#include "MinHeap.h"
#include "AVLTree.h"

using namespace std;

// ============================================================================
// MINI GIS ENGINE - Menu-Driven Spatial Query System
// ============================================================================
// Data Structures Used:
//   - QuadTree  (Unit 5) : 2D spatial indexing, range & radius queries
//   - MinHeap   (Unit 2) : K-Nearest Neighbor search
//   - AVL Tree  (Unit 1) : Sorted point storage, 1D range search
// ============================================================================

// Global data structures
QuadTree* quadTree = nullptr;
AVLTree avlTree;

// World boundary for the QuadTree (can be adjusted)
const double WORLD_MIN = -1000.0;
const double WORLD_MAX = 1000.0;

// ============================================================================
// Helper Functions
// ============================================================================

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void pressEnterToContinue() {
    cout << "\n  Press Enter to continue...";
    cin.get();
}



void printMenu() {
    cout << "\n";
    cout << "  +------------------------------------------------------+\n";
    cout << "  |                    MAIN MENU                         |\n";
    cout << "  +------------------------------------------------------+\n";
    cout << "  |  1.  Add a Point                                     |\n";
    cout << "  |  2.  Display All Points (Sorted by X - AVL Tree)     |\n";
    cout << "  |  3.  Find Distance Between Two Points                |\n";
    cout << "  |  4.  Find All Points Near a Coordinate (Radius)      |\n";
    cout << "  |  5.  Find K-Nearest Neighbors (KNN)                  |\n";
    cout << "  |  6.  Calculate Area of Polygon (Shoelace)            |\n";
    cout << "  |  7.  Remove a Point                                  |\n";
    cout << "  |  8.  Visualize QuadTree Partitions                   |\n";
    cout << "  |  9.  Display AVL Tree Structure                      |\n";
    cout << "  |  0.  Exit                                            |\n";
    cout << "  +------------------------------------------------------+\n";
    cout << "  Enter choice: ";
}

// ============================================================================
// Menu Option Implementations
// ============================================================================

// Option 1: Add a Point
void addPoint() {
    cout << "\n  -- Add a New Point ----------------------------------\n";

    string label;
    double x, y;

    cout << "  Enter label (name): ";
    getline(cin, label);

    cout << "  Enter X coordinate: ";
    cin >> x;
    cout << "  Enter Y coordinate: ";
    cin >> y;
    clearInput();

    // Validate coordinates are within world bounds
    if (x < WORLD_MIN || x > WORLD_MAX || y < WORLD_MIN || y > WORLD_MAX) {
        cout << "\n  [ERROR] Coordinates must be within ["
             << WORLD_MIN << ", " << WORLD_MAX << "]\n";
        return;
    }

    Point p(x, y, label);

    // Insert into both data structures
    bool qtInserted = quadTree->insert(p);
    avlTree.insert(p);

    if (qtInserted) {
        cout << "\n  [OK] Point added: " << p << endl;
        cout << "       -> Inserted into QuadTree and AVL Tree\n";
    } else {
        cout << "\n  [ERROR] Failed to insert into QuadTree.\n";
    }
}

// Option 2: Display All Points sorted by X
void displayAllPoints() {
    cout << "\n  -- All Points (Sorted by X - AVL Inorder) ----------\n\n";

    vector<Point> sorted = avlTree.inorder();

    if (sorted.empty()) {
        cout << "  No points stored yet.\n";
        return;
    }

    cout << "  " << left << setw(5) << "#"
         << setw(15) << "Label"
         << setw(12) << "X"
         << setw(12) << "Y" << endl;
    cout << "  " << string(44, '-') << endl;

    for (int i = 0; i < (int)sorted.size(); i++) {
        cout << "  " << left << setw(5) << (i + 1)
             << setw(15) << sorted[i].label
             << setw(12) << fixed << setprecision(2) << sorted[i].x
             << setw(12) << sorted[i].y << endl;
    }

    cout << "\n  Total: " << sorted.size() << " point(s)\n";
}

// Option 3: Find Distance Between Two Points
void findDistance() {
    cout << "\n  -- Distance Between Two Points ----------------------\n";

    string label1, label2;
    cout << "  Enter label of Point 1: ";
    getline(cin, label1);
    cout << "  Enter label of Point 2: ";
    getline(cin, label2);

    Point p1, p2;
    bool found1 = avlTree.findByLabel(label1, p1);
    bool found2 = avlTree.findByLabel(label2, p2);

    if (!found1) {
        cout << "\n  [ERROR] Point '" << label1 << "' not found.\n";
        return;
    }
    if (!found2) {
        cout << "\n  [ERROR] Point '" << label2 << "' not found.\n";
        return;
    }

    cout << "\n  Point 1: " << p1 << endl;
    cout << "  Point 2: " << p2 << endl;

    double eucDist = Distance::euclidean(p1, p2);
    double manDist = Distance::manhattan(p1, p2);

    cout << "\n  +-------------------------------------+\n";
    cout << "  |  Euclidean Distance: " << fixed << setprecision(4)
         << setw(14) << eucDist << " |\n";
    cout << "  |  Manhattan Distance: " << setw(14) << manDist << " |\n";
    cout << "  +-------------------------------------+\n";
}

// Option 4: Find All Points Near a Coordinate (Radius Query)
void findNearbyPoints() {
    cout << "\n  -- Radius Query (QuadTree) --------------------------\n";

    double cx, cy, radius;
    cout << "  Enter center X: ";
    cin >> cx;
    cout << "  Enter center Y: ";
    cin >> cy;
    cout << "  Enter search radius: ";
    cin >> radius;
    clearInput();

    if (radius <= 0) {
        cout << "\n  [ERROR] Radius must be positive.\n";
        return;
    }

    Point center(cx, cy, "QueryCenter");
    vector<Point> found;
    quadTree->queryRadius(center, radius, found);

    cout << "\n  Search center: (" << cx << ", " << cy << ")"
         << "  Radius: " << radius << endl;
    cout << "  --------------------------------------------\n";

    if (found.empty()) {
        cout << "  No points found within radius.\n";
    } else {
        cout << "  Found " << found.size() << " point(s):\n\n";
        for (int i = 0; i < (int)found.size(); i++) {
            double dist = Distance::euclidean(center, found[i]);
            cout << "    " << (i + 1) << ". " << found[i]
                 << "  (dist: " << fixed << setprecision(2) << dist << ")\n";
        }
    }
}

// Option 5: K-Nearest Neighbors (using MinHeap)
void findKNN() {
    cout << "\n  -- K-Nearest Neighbors (MinHeap) -------------------\n";

    double qx, qy;
    int k;
    cout << "  Enter query X: ";
    cin >> qx;
    cout << "  Enter query Y: ";
    cin >> qy;
    cout << "  Enter K (number of neighbors): ";
    cin >> k;
    clearInput();

    if (k <= 0) {
        cout << "\n  [ERROR] K must be positive.\n";
        return;
    }

    Point query(qx, qy, "Query");

    // Step 1: Get ALL points from the QuadTree
    vector<Point> allPoints;
    quadTree->getAllPoints(allPoints);

    if (allPoints.empty()) {
        cout << "\n  No points stored yet.\n";
        return;
    }

    if (k > (int)allPoints.size()) {
        cout << "\n  [NOTE] Only " << allPoints.size()
             << " points exist. Showing all.\n";
        k = (int)allPoints.size();
    }

    // Step 2: Insert all (distance, point) pairs into MinHeap
    MinHeap heap;
    for (const Point& p : allPoints) {
        double dist = Distance::euclidean(query, p);
        heap.insert(dist, p);
    }

    // Step 3: Extract K nearest
    cout << "\n  Query point: (" << qx << ", " << qy << ")";
    cout << "  K = " << k << endl;
    cout << "  --------------------------------------------\n";
    cout << "  " << k << " Nearest Neighbor(s):\n\n";

    for (int i = 0; i < k; i++) {
        auto nearest = heap.extractMin();
        cout << "    " << (i + 1) << ". " << nearest.second
             << "  (dist: " << fixed << setprecision(4)
             << nearest.first << ")\n";
    }
}

// Option 6: Calculate Area of Polygon (Shoelace Formula)
void calculateArea() {
    cout << "\n  -- Area of Polygon (Shoelace Formula) --------------\n";

    int n;
    cout << "  Enter number of vertices: ";
    cin >> n;
    clearInput();

    if (n < 3) {
        cout << "\n  [ERROR] Need at least 3 vertices for a polygon.\n";
        return;
    }

    vector<Point> vertices;
    cout << "\n  Enter vertex labels in ORDER (clockwise or counter-clockwise):\n";

    for (int i = 0; i < n; i++) {
        string label;
        cout << "    Vertex " << (i + 1) << " label: ";
        getline(cin, label);

        Point p;
        if (!avlTree.findByLabel(label, p)) {
            cout << "\n  [ERROR] Point '" << label << "' not found.\n";
            return;
        }
        vertices.push_back(p);
    }

    // Shoelace Formula:
    // Area = 0.5 * |Σ(x_i * y_{i+1} - x_{i+1} * y_i)|
    // where indices wrap around (n connects back to 0)
    double area = 0.0;
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        area += vertices[i].x * vertices[j].y;
        area -= vertices[j].x * vertices[i].y;
    }
    area = fabs(area) / 2.0;

    cout << "\n  Polygon vertices:\n";
    for (int i = 0; i < n; i++) {
        cout << "    " << (i + 1) << ". " << vertices[i] << endl;
    }

    cout << "\n  +-------------------------------------+\n";
    cout << "  |  Area = " << fixed << setprecision(4)
         << setw(28) << area << " |\n";
    cout << "  +-------------------------------------+\n";
}

// Option 7: Remove a Point
void removePoint() {
    cout << "\n  -- Remove a Point ----------------------------------\n";

    string label;
    cout << "  Enter label of point to remove: ";
    getline(cin, label);

    bool removed = avlTree.remove(label);

    if (removed) {
        // NOTE: QuadTree doesn't support efficient deletion.
        // We rebuild it from remaining points in the AVL Tree.
        delete quadTree;
        double hw = (WORLD_MAX - WORLD_MIN) / 2.0;
        double center = (WORLD_MAX + WORLD_MIN) / 2.0;
        quadTree = new QuadTree(AABB(center, center, hw, hw), 4);

        vector<Point> remaining = avlTree.inorder();
        for (const Point& p : remaining) {
            quadTree->insert(p);
        }

        cout << "\n  [OK] Point '" << label << "' removed.\n";
        cout << "       -> Removed from AVL Tree, QuadTree rebuilt.\n";
    } else {
        cout << "\n  [ERROR] Point '" << label << "' not found.\n";
    }
}

// Option 8: Visualize QuadTree
void visualizeQuadTree() {
    cout << "\n  -- QuadTree Structure -------------------------------\n\n";

    if (quadTree->size() == 0) {
        cout << "  QuadTree is empty.\n";
        return;
    }

    cout << "  Total points: " << quadTree->size() << "\n\n";
    quadTree->display();
}

// Option 9: Display AVL Tree Structure
void displayAVLTree() {
    cout << "\n  -- AVL Tree Structure ------------------------------\n\n";
    avlTree.display();
}

// ============================================================================
// Main - Entry Point
// ============================================================================

int main() {
#ifdef _WIN32
    // Enforce UTF-8 encoding for Windows console to correctly render box characters
    SetConsoleOutputCP(CP_UTF8);
#endif

    // Initialize QuadTree with world boundaries
    double hw = (WORLD_MAX - WORLD_MIN) / 2.0;
    double center = (WORLD_MAX + WORLD_MIN) / 2.0;
    quadTree = new QuadTree(AABB(center, center, hw, hw), 4);

    
    int choice;
    bool running = true;

    while (running) {
        printMenu();
        cin >> choice;
        clearInput();

        switch (choice) {
            case 1:
                addPoint();
                break;
            case 2:
                displayAllPoints();
                break;
            case 3:
                findDistance();
                break;
            case 4:
                findNearbyPoints();
                break;
            case 5:
                findKNN();
                break;
            case 6:
                calculateArea();
                break;
            case 7:
                removePoint();
                break;
            case 8:
                visualizeQuadTree();
                break;
            case 9:
                displayAVLTree();
                break;
            case 0:
                cout << "\n  Exiting GIS Engine. Goodbye!\n\n";
                running = false;
                break;
            default:
                cout << "\n  [ERROR] Invalid choice. Try again.\n";
        }

        if (running) {
            pressEnterToContinue();
        }
    }

    // Cleanup
    delete quadTree;

    return 0;
}
