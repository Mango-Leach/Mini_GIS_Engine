// GISEngine.js — translated from main.cpp (logic only, no UI)
// Owns all data structure instances and exposes clean operations.
// The frontend calls these methods; nothing here touches the DOM.

import { Point }     from './point.js';
import { Distance }  from './distance.js';
import { AVLTree }   from './avltree.js';
import { QuadTree, AABB } from './quadtree.js';
import { MinHeap }   from './minheap.js';
import { Trie }           from './trie.js';
import { SuffixTrieDAWG } from './suffixtrie.js';

const WORLD_MIN = -1000;
const WORLD_MAX =  1000;

export class GISEngine {
    constructor() {
        this.avlTree    = new AVLTree();
        this.trie       = new Trie();
        this.suffixDAWG = new SuffixTrieDAWG();
        this._initQuadTree();
    }

    _initQuadTree() {
        const hw     = (WORLD_MAX - WORLD_MIN) / 2;
        const center = (WORLD_MAX + WORLD_MIN) / 2;
        this.quadTree = new QuadTree(new AABB(center, center, hw, hw), 4);
    }

    // -------------------------------------------------------------------------
    // Add a point
    // Returns { ok: bool, message: string }
    // -------------------------------------------------------------------------
    addPoint(x, y, label) {
        if (x < WORLD_MIN || x > WORLD_MAX || y < WORLD_MIN || y > WORLD_MAX) {
            return { ok: false, message: `Coordinates must be within [${WORLD_MIN}, ${WORLD_MAX}]` };
        }

        const point = new Point(x, y, label);
        this.quadTree.insert(point);
        this.avlTree.insert(point);
        this.trie.insert(label, point);
        this.suffixDAWG.insert(label, point);

        return { ok: true, message: `Point added: ${point.toString()}` };
    }

    // -------------------------------------------------------------------------
    // Remove a point by label
    // Returns { ok: bool, message: string }
    // -------------------------------------------------------------------------
    removePoint(label) {
        const removed = this.avlTree.remove(label);

        if (!removed) {
            return { ok: false, message: `Point '${label}' not found` };
        }

        this.trie.delete(label);
        this.suffixDAWG.delete(label);

        // QuadTree has no delete — rebuild from remaining AVL points
        this._initQuadTree();
        for (const p of this.avlTree.inorder()) {
            this.quadTree.insert(p);
        }

        return { ok: true, message: `Point '${label}' removed` };
    }

    // -------------------------------------------------------------------------
    // Get all points sorted by X (AVL inorder)
    // Returns array of Point
    // -------------------------------------------------------------------------
    getAllPointsSorted() {
        return this.avlTree.inorder();
    }

    // -------------------------------------------------------------------------
    // Distance between two points by label
    // Returns { ok, euclidean, manhattan, p1, p2, message }
    // -------------------------------------------------------------------------
    getDistance(label1, label2) {
        const p1 = this.avlTree.findByLabel(label1);
        const p2 = this.avlTree.findByLabel(label2);

        if (!p1) return { ok: false, message: `Point '${label1}' not found` };
        if (!p2) return { ok: false, message: `Point '${label2}' not found` };

        return {
            ok:        true,
            p1,
            p2,
            euclidean: Distance.euclidean(p1, p2),
            manhattan: Distance.manhattan(p1, p2),
        };
    }

    // -------------------------------------------------------------------------
    // Radius query — all points within radius of (cx, cy)
    // Returns { ok, points, message }
    // -------------------------------------------------------------------------
    queryRadius(cx, cy, radius) {
        if (radius <= 0) return { ok: false, message: 'Radius must be positive' };

        const center = new Point(cx, cy, 'QueryCenter');
        const points = this.quadTree.queryRadius(center, radius);

        // Attach actual distances for display
        const withDist = points.map(p => ({
            point:    p,
            distance: Distance.euclidean(center, p),
        }));
        withDist.sort((a, b) => a.distance - b.distance);

        return { ok: true, points: withDist };
    }

    // -------------------------------------------------------------------------
    // KNN — K nearest neighbors
    // Returns { ok, neighbors, message }
    // -------------------------------------------------------------------------
    queryKNN(qx, qy, k) {
        if (k <= 0) return { ok: false, message: 'K must be positive' };

        const allPoints = this.quadTree.getAllPoints();
        if (allPoints.length === 0) return { ok: false, message: 'No points stored' };

        const actualK = Math.min(k, allPoints.length);
        const query   = new Point(qx, qy, 'Query');
        const heap    = new MinHeap();

        for (const p of allPoints) {
            heap.insert(Distance.euclidean(query, p), p);
        }

        const neighbors = [];
        for (let i = 0; i < actualK; i++) {
            neighbors.push(heap.extractMin());
        }

        return { ok: true, neighbors };
    }

    // -------------------------------------------------------------------------
    // Polygon area — Shoelace formula on an ordered list of labels
    // Returns { ok, area, vertices, message }
    // -------------------------------------------------------------------------
    calculateArea(labels) {
        if (labels.length < 3) {
            return { ok: false, message: 'Need at least 3 vertices' };
        }

        const vertices = [];
        for (const label of labels) {
            const p = this.avlTree.findByLabel(label);
            if (!p) return { ok: false, message: `Point '${label}' not found` };
            vertices.push(p);
        }

        let area = 0;
        const n  = vertices.length;
        for (let i = 0; i < n; i++) {
            const j  = (i + 1) % n;
            area    += vertices[i].x * vertices[j].y;
            area    -= vertices[j].x * vertices[i].y;
        }
        area = Math.abs(area) / 2;

        return { ok: true, area, vertices };
    }

    // -------------------------------------------------------------------------
    // Trie — find point by exact label
    // Returns Point or null
    // -------------------------------------------------------------------------
    searchLabel(label) {
        return this.trie.search(label);
    }

    // -------------------------------------------------------------------------
    // Unified search — prefix (Trie) + substring (SuffixDAWG) + fuzzy
    // Returns array of { point, matchType } ranked: prefix > substring > fuzzy
    // -------------------------------------------------------------------------
    search(query) {
        if (!query) return [];

        const results = new Map(); // label → { point, rank }

        // 1. Prefix match via Trie (rank 0 — best)
        for (const p of this.trie.autocomplete(query)) {
            results.set(p.label, { point: p, rank: 0 });
        }

        // 2. Substring match via SuffixDAWG (rank 1)
        for (const p of this.suffixDAWG.substringSearch(query)) {
            if (!results.has(p.label)) {
                results.set(p.label, { point: p, rank: 1 });
            }
        }

        // 3. Fuzzy match, maxEdits=1 (rank 2+dist — lowest priority)
        for (const { point, dist } of this.suffixDAWG.fuzzySearch(query, 1)) {
            if (!results.has(point.label)) {
                results.set(point.label, { point, rank: 2 + dist });
            }
        }

        return [...results.values()]
            .sort((a, b) => a.rank - b.rank)
            .map(e => e.point);
    }

    // -------------------------------------------------------------------------
    // Trie — autocomplete prefix (kept for backwards compat)
    // -------------------------------------------------------------------------
    autocomplete(prefix) {
        if (!prefix) return [];
        return this.trie.autocomplete(prefix);
    }

    // -------------------------------------------------------------------------
    // QuadTree boundaries — for rendering partition lines
    // Returns array of AABB
    // -------------------------------------------------------------------------
    getQuadTreeBoundaries() {
        return this.quadTree.getBoundaries();
    }

    get worldMin() { return WORLD_MIN; }
    get worldMax() { return WORLD_MAX; }
}