class TrieNode {
    constructor() {
        this.children  = {};  
        this.isEnd     = false;
        this.pointData = null; 
    }
}

export class Trie {
    constructor() {
        this.root = new TrieNode();
    }

    insert(label, point) {
        let node = this.root;
        for (const ch of label) {
            if (!node.children[ch]) {
                node.children[ch] = new TrieNode();
            }
            node = node.children[ch];
        }
        node.isEnd     = true;
        node.pointData = point;
    }

    search(label) {
        let node = this.root;
        for (const ch of label) {
            if (!node.children[ch]) return null;
            node = node.children[ch];
        }
        return node.isEnd ? node.pointData : null;
    }

    autocomplete(prefix) {
        let node = this.root;
        for (const ch of prefix) {
            if (!node.children[ch]) return [];
            node = node.children[ch];
        }
        const results = [];
        this._collect(node, results);
        return results;
    }

    _collect(node, results) {
        if (node.isEnd) results.push(node.pointData);
        for (const ch of Object.keys(node.children)) {
            this._collect(node.children[ch], results);
        }
    }

    delete(label) {
        return this._delete(this.root, label, 0);
    }

    _delete(node, label, depth) {
        if (!node) return false;

        if (depth === label.length) {
            if (!node.isEnd) return false;
            node.isEnd     = false;
            node.pointData = null;
            return true;
        }

        const ch = label[depth];
        if (!node.children[ch]) return false;

        const deleted = this._delete(node.children[ch], label, depth + 1);

        if (deleted &&
            !node.children[ch].isEnd &&
            Object.keys(node.children[ch].children).length === 0) {
            delete node.children[ch];
        }

        return deleted;
    }

    isEmpty() {
        return Object.keys(this.root.children).length === 0;
    }
}