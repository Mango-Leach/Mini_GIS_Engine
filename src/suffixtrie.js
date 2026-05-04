class SuffixNode {
    constructor() {
        this.children = {};      
        this.isEnd    = false;
        this.labels   = new Set();
    }
}

export class SuffixTrieDAWG {
    constructor() {
        this.root   = new SuffixNode();
        this._labels = new Map(); 
    }

    insert(label, point) {
        this._labels.set(label, point);
        this._rebuild();
    }

    delete(label) {
        if (!this._labels.has(label)) return false;
        this._labels.delete(label);
        this._rebuild();
        return true;
    }

    _rebuild() {
        this.root = new SuffixNode();

        for (const [label, point] of this._labels) {
            for (let i = 0; i < label.length; i++) {
                this._insertSuffix(label.slice(i), label);
            }
        }

        this._minimise(this.root, new Map());
    }

    _insertSuffix(suffix, originalLabel) {
        let node = this.root;
        for (const ch of suffix) {
            if (!node.children[ch]) node.children[ch] = new SuffixNode();
            node = node.children[ch];
        }
        node.isEnd = true;
        node.labels.add(originalLabel);
    }

    _minimise(node, registry) {
        for (const ch of Object.keys(node.children)) {
            node.children[ch] = this._minimise(node.children[ch], registry);
        }
        const sig = this._sig(node);
        if (registry.has(sig)) return registry.get(sig);
        registry.set(sig, node);
        return node;
    }

    _sig(node) {
        const childPart = Object.entries(node.children)
            .sort(([a], [b]) => a < b ? -1 : 1)
            .map(([ch, child]) => `${ch}:${this._nodeId(child)}`)
            .join(',');
        const labelPart = [...node.labels].sort().join('|');
        return `${node.isEnd}:${labelPart}:${childPart}`;
    }

    _nodeId(node) {
        if (!SuffixTrieDAWG._ids) SuffixTrieDAWG._ids = new WeakMap();
        if (!SuffixTrieDAWG._ids.has(node)) {
            SuffixTrieDAWG._ids.set(node, SuffixTrieDAWG._counter = (SuffixTrieDAWG._counter || 0) + 1);
        }
        return SuffixTrieDAWG._ids.get(node);
    }

    substringSearch(query) {
        let node = this.root;
        for (const ch of query) {
            if (!node.children[ch]) return [];
            node = node.children[ch];
        }
        const labelSet = new Set();
        this._collectLabels(node, labelSet);
        return [...labelSet]
            .filter(lbl => this._labels.has(lbl))
            .map(lbl => this._labels.get(lbl));
    }

    _collectLabels(node, set) {
        if (node.isEnd) for (const lbl of node.labels) set.add(lbl);
        for (const child of Object.values(node.children)) {
            this._collectLabels(child, set);
        }
    }

    fuzzySearch(query, maxEdits = 1) {
        const startRow = Array.from({ length: query.length + 1 }, (_, i) => i);
        const hits     = new Map();

        this._fuzzyNode(this.root, query, startRow, maxEdits, hits);

        return [...hits.entries()]
            .filter(([lbl]) => this._labels.has(lbl))
            .sort(([, a], [, b]) => a - b)
            .map(([lbl, dist]) => ({ point: this._labels.get(lbl), dist }));
    }

    _fuzzyNode(node, query, prevRow, maxEdits, hits) {
        if (node.isEnd) {
            const dist = prevRow[query.length];
            if (dist <= maxEdits) {
                for (const lbl of node.labels) {
                    if (!hits.has(lbl) || hits.get(lbl) > dist) hits.set(lbl, dist);
                }
            }
        }

        for (const [ch, child] of Object.entries(node.children)) {
            const currRow = [prevRow[0] + 1];
            for (let i = 1; i <= query.length; i++) {
                const ins     = currRow[i - 1] + 1;
                const del     = prevRow[i] + 1;
                const replace = prevRow[i - 1] + (ch !== query[i - 1] ? 1 : 0);
                currRow.push(Math.min(ins, del, replace));
            }
            if (Math.min(...currRow) <= maxEdits) {
                this._fuzzyNode(child, query, currRow, maxEdits, hits);
            }
        }
    }

    isEmpty() { return this._labels.size === 0; }
}