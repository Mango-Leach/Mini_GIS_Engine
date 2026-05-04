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

    removePoint(label) {
        const removed = this.avlTree.remove(label);

        if (!removed) {
            return { ok: false, message: `Point '${label}' not found` };
        }

        this.trie.delete(label);
        this.suffixDAWG.delete(label);

        this._initQuadTree();
        for (const p of this.avlTree.inorder()) {
            this.quadTree.insert(p);
        }

        return { ok: true, message: `Point '${label}' removed` };
    }

    getAllPointsSorted() {
        return this.avlTree.inorder();
    }

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

    queryRadius(cx, cy, radius) {
        if (radius <= 0) return { ok: false, message: 'Radius must be positive' };

        const center = new Point(cx, cy, 'QueryCenter');
        const points = this.quadTree.queryRadius(center, radius);

        const withDist = points.map(p => ({
            point:    p,
            distance: Distance.euclidean(center, p),
        }));
        withDist.sort((a, b) => a.distance - b.distance);

        return { ok: true, points: withDist };
    }

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

    searchLabel(label) {
        return this.trie.search(label);
    }

    search(query) {
        if (!query) return [];

        const results = new Map();

        for (const p of this.trie.autocomplete(query)) {
            results.set(p.label, { point: p, rank: 0 });
        }

        for (const p of this.suffixDAWG.substringSearch(query)) {
            if (!results.has(p.label)) {
                results.set(p.label, { point: p, rank: 1 });
            }
        }

        for (const { point, dist } of this.suffixDAWG.fuzzySearch(query, 1)) {
            if (!results.has(point.label)) {
                results.set(point.label, { point, rank: 2 + dist });
            }
        }

        return [...results.values()]
            .sort((a, b) => a.rank - b.rank)
            .map(e => e.point);
    }

    autocomplete(prefix) {
        if (!prefix) return [];
        return this.trie.autocomplete(prefix);
    }

    getQuadTreeBoundaries() {
        return this.quadTree.getBoundaries();
    }

    get worldMin() { return WORLD_MIN; }
    get worldMax() { return WORLD_MAX; }
}