# GIS Engine

A browser-based spatial query sandbox that visualises classic data structures running live in the browser. Points are placed on a real Leaflet map (centred on Pune) and every operation — from nearest-neighbour search to polygon area — is powered by hand-rolled implementations of AVL trees, QuadTrees, MinHeaps, and Tries, all written in vanilla JavaScript ES modules.

---

## Project Overview

The project has two distinct layers that never touch each other:

- **Engine layer** (`src/`) — pure data-structure logic, zero DOM, importable anywhere.
- **UI layer** (`index.html` + `main.css`) — Leaflet map, panel, modals, and canvas overlays. Calls engine methods and renders results.

```
gis-engine/
├── index.html          # Shell, Leaflet bootstrap, all UI event wiring
├── main.css            # Dark-panel theme, Leaflet overrides
└── src/
    ├── gisengine.js    # Façade — the only file the UI imports
    ├── point.js        # 2-D point with comparators
    ├── avltree.js      # Self-balancing BST (sorted by x, y)
    ├── quadtree.js     # Recursive 2-D spatial index + AABB
    ├── minheap.js      # Binary min-heap for KNN extraction
    ├── trie.js         # Prefix trie (exact search + autocomplete)
    ├── suffixtrie.js   # Suffix trie with DAWG + Levenshtein fuzzy search
    └── distance.js     # Static Euclidean / Manhattan helpers
```

---

## Coordinate System

World space is `[-1000, 1000]` on both axes. The UI maps this onto real geography via a linear transform centred on Pune (18.5204° N, 73.8567° E):

```
lat = PUNE.lat + y * SCALE      (SCALE = 0.000045 °/unit → ±1000 ≈ ±4.5 km)
lng = PUNE.lng + x * SCALE
```

One world unit is roughly 5 metres. The radius query converts world-unit radius to metres for `L.circle` via `r * SCALE * 111320`.

---

## Engine API — `src/gisengine.js`

`GISEngine` is the single façade the UI talks to. It owns all data-structure instances and rebuilds the QuadTree on every deletion (QuadTree has no native delete).

### `addPoint(x, y, label) → { ok, message }`

Validates that `x` and `y` are inside `[-1000, 1000]`, then inserts the point into all four structures simultaneously: QuadTree, AVL tree, prefix Trie, and suffix DAWG. Returns `ok: false` with an error message if coordinates are out of range or the label is a duplicate.

### `removePoint(label) → { ok, message }`

Removes the point from the AVL tree and both tries. Because the QuadTree has no delete operation, it is re-initialised and every remaining AVL point is re-inserted. This is O(n) but fine for the dataset sizes this tool targets.

### `getAllPointsSorted() → Point[]`

Returns an in-order traversal of the AVL tree — points sorted by x-coordinate (y as tiebreaker). Used to populate the results panel and to redraw markers.

### `getDistance(label1, label2) → { ok, p1, p2, euclidean, manhattan }`

Looks up both points by label using the AVL tree's full-tree label scan, then delegates to `Distance.euclidean` and `Distance.manhattan`. Both metrics are returned so the UI can display them side-by-side.

### `queryRadius(cx, cy, radius) → { ok, points }`

Constructs a temporary `QueryCenter` point and delegates to `QuadTree.queryRadius`, which prunes subtrees whose bounding boxes don't intersect the query circle. Results are enriched with Euclidean distances and sorted nearest-first before returning.

### `queryKNN(qx, qy, k) → { ok, neighbors }`

Fetches all points from the QuadTree, inserts every `{ distance, point }` pair into a `MinHeap`, then pops the minimum `k` times. Clamps `k` to the number of available points. This is a brute-force KNN; for large datasets the QuadTree traversal itself could be made branch-and-bound, but the heap extraction is always O(k log n).

### `calculateArea(labels) → { ok, area, vertices }`

Resolves each label to a `Point` via the AVL tree, then applies the **Shoelace formula** (Gauss's area formula) over the ordered vertex list. The result is the absolute value of half the signed area, so vertex winding direction does not matter.

### `searchLabel(label) → Point | null`

Exact lookup through the prefix Trie in O(|label|) time.

### `search(query) → Point[]`

Unified, ranked search used by the autocomplete input:

1. **Prefix match** via `Trie.autocomplete` — rank 0 (best). Finds all labels that *start with* the query.
2. **Substring match** via `SuffixTrieDAWG.substringSearch` — rank 1. The suffix trie stores every suffix of every label, so "pha" matches "Alpha".
3. **Fuzzy match** via `SuffixTrieDAWG.fuzzySearch(query, maxEdits=1)` — rank 2+dist. Levenshtein DP over the trie nodes with early branch pruning.

Results de-duplicate by label and are returned in rank order.

### `getQuadTreeBoundaries() → AABB[]`

Recursively collects every bounding box in the QuadTree for the "QuadTree Lines" overlay. Each `AABB` has `cx, cy, halfWidth, halfHeight`.

---

## Data Structures — `src/`

### AVL Tree (`avltree.js`)

A self-balancing binary search tree. Points are sorted by `x` (then `y` as a tiebreaker). Balance is maintained via left/right rotations after every insert or delete, keeping height ≤ 1.44 log₂(n). The tree supports:

- `insert(point)` — standard BST insert + rebalance upward.
- `remove(label)` — label-keyed removal via full-tree scan (label is not the sort key). On a two-child node, the inorder successor replaces the deleted node.
- `inorder()` — yields all points sorted by x.
- `rangeSearch(xMin, xMax)` — 1-D x-range query that prunes left/right subtrees early.
- `findByLabel(label)` — full traversal to locate by label in O(n) worst case.

### QuadTree (`quadtree.js`)

A recursive 2-D spatial index. Each node holds up to `capacity` (default 4) points before splitting into NW/NE/SW/SE children. The bounding box is an `AABB` (axis-aligned bounding box) with intersection and containment tests used to prune queries.

- `insert(point)` — places the point in the first leaf whose box contains it; subdivides if full.
- `queryRange(aabb)` — rectangle query: skips any subtree whose box doesn't intersect the query.
- `queryRadius(center, radius)` — uses a square AABB as a pre-filter, then checks true Euclidean distance.
- `getAllPoints()` — full traversal, used by KNN.
- `getBoundaries()` — collects every AABB for the grid overlay.

### MinHeap (`minheap.js`)

A standard binary min-heap over `{ distance, point }` pairs, ordered by distance. Array-backed with index math (`parent = ⌊(i−1)/2⌋`). Used by KNN: all n points are inserted in O(n log n), then k minimums are extracted in O(k log n).

### Trie (`trie.js`)

A character-level prefix trie. Each node has a `children` map (char → TrieNode) and an `isEnd` flag storing the associated `Point`. Supports exact search in O(|label|), prefix autocomplete via subtree collection, and deletion with dead-branch cleanup.

### Suffix Trie + DAWG (`suffixtrie.js`)

Every label's suffixes are inserted into a second trie (e.g. "Alpha" → "Alpha", "lpha", "pha", "ha", "a"). This allows O(|query|) substring lookup of any infix. After each modification the trie is **minimised into a DAWG** (Directed Acyclic Word Graph): structurally identical subtrees are merged by a post-order signature walk, reducing node count.

Fuzzy search runs Levenshtein DP row-by-row through the trie nodes, pruning any branch where `min(currentRow) > maxEdits`. With `maxEdits=1` this catches single-character typos cheaply.

### Distance (`distance.js`)

Three static methods: `euclidean`, `manhattan`, and `euclideanSquared` (sqrt-free, for comparisons only). No state.

---

## UI & Business Logic — `index.html`

### Map Setup

Leaflet is initialised with map dragging **disabled** (`dragging: false`). Instead, a custom right-click-drag panning handler is wired directly to `mousedown/mousemove/mouseup` on the map `<div>`, so left-click is free for tool interactions. The tile layer is CartoDB Light.

### Coordinate Transform

`w2ll(wx, wy)` converts world coordinates to a Leaflet `LatLng`. `ll2w(lat, lng)` is the inverse. `wRadToM(r)` converts a world-unit radius to metres for `L.circle`.

### Layer Groups

Three separate `L.layerGroup` instances keep concerns separated:
- `ptLayer` — all point markers and their text labels.
- `ovLayer` — transient query overlays (radius circle, KNN spokes, distance line, polygon fill).
- `qtLayer` — QuadTree rectangle grid.

### Marker Factory (`mkMarker`)

Each point gets a `L.circleMarker` (the dot) plus a `L.marker` carrying a `L.divIcon` (the text label, rendered as a positioned `<span>`). Both are wrapped in a `L.layerGroup` with a custom `setStyle` shim so callers can recolour the circle without knowing the internal structure.

### Mode System

A single `mode` string (`add`, `remove`, `radius`, `knn`, `distance`, `polygon`) controls what happens on map click and marker click. Switching mode calls `clearQuery()`, which clears all overlays, resets marker colours, empties `distancePair` and `polyPoints`, and redraws the results panel.

### Add Flow

Map click → `openAddModal` populates a modal with the clicked world coordinates pre-filled. On confirm, `engine.addPoint` is called; if successful, `addMarker` creates the Leaflet marker, `redrawQT` refreshes the QuadTree grid, and `refreshList` updates the panel.

### Radius Query Flow

Map click → modal asks for radius → `engine.queryRadius` → `drawRadiusOverlay` draws a `L.circle` at the correct metre radius plus a centre dot, then recolours hit markers green.

### KNN Flow

Map click → modal asks for K → `engine.queryKNN` → `drawKNNOverlay` draws spoke polylines from the query centre to each neighbour and a purple centre marker. Hit markers turn green.

### Distance Flow

First marker click stores label in `distancePair` and turns the marker purple. Second marker click calls `engine.getDistance`, draws a dashed polyline between the two points, and shows both Euclidean and Manhattan distances in the panel.

### Polygon Flow

Each marker click appends the point to `polyPoints` and recolours it amber. `drawPolyPreview` redraws a dashed polyline through the in-progress vertices. Pressing **Enter** calls `finishPolygon`: `engine.calculateArea` runs the Shoelace formula, a filled `L.polygon` replaces the preview line, and an area label is placed at the centroid via a `L.divIcon`. **Escape** cancels and resets colours.

### Search / Autocomplete

The search input calls `engine.search(query)` (prefix + substring + fuzzy, ranked) and renders up to 8 autocomplete items. Clicking an item highlights the point on the map, pans to it, and selects it in the results panel.

### Results Panel

`refreshList` renders all points from `engine.getAllPointsSorted()`. Query-specific helpers (`showRadiusResults`, `showKNNResults`, `showDistResults`, `showAreaResults`) replace the list content with result-specific rows and badges. Every row with a `data-label` attribute gets a click handler that highlights the marker and pans the map.

---

