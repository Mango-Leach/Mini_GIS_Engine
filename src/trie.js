// Trie.js — new addition (Unit 3)
// Stores point labels character by character.
// Supports: insert, exact search, prefix autocomplete, delete.

class TrieNode {
    constructor() {
        this.children  = {};  // char -> TrieNode
        this.isEnd     = false;
        this.pointData = null; // stores the Point when isEnd is true
    }
}

export class Trie {
    constructor() {
        this.root = new TrieNode();
    }

    // Insert a label and associate it with a Point
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

    // Exact search — returns the Point if label exists, null otherwise
    search(label) {
        let node = this.root;
        for (const ch of label) {
            if (!node.children[ch]) return null;
            node = node.children[ch];
        }
        return node.isEnd ? node.pointData : null;
    }

    // Prefix search — returns all Points whose label starts with prefix
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

    // Collect all Points in the subtree rooted at node
    _collect(node, results) {
        if (node.isEnd) results.push(node.pointData);
        for (const ch of Object.keys(node.children)) {
            this._collect(node.children[ch], results);
        }
    }

    // Delete a label — returns true if removed, false if not found
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

        // Clean up child node if it has no children and is not an end node
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