class AVLNode {
    constructor(point) {
        this.data   = point;
        this.left   = null;
        this.right  = null;
        this.height = 1;
    }
}

export class AVLTree {
    constructor() {
        this.root = null;
    }

    _height(node) {
        return node ? node.height : 0;
    }

    _balanceFactor(node) {
        return node ? this._height(node.left) - this._height(node.right) : 0;
    }

    _updateHeight(node) {
        if (node) {
            node.height = 1 + Math.max(this._height(node.left), this._height(node.right));
        }
    }

    _rotateRight(y) {
        const x = y.left;
        const B = x.right;
        x.right = y;
        y.left  = B;
        this._updateHeight(y);
        this._updateHeight(x);
        return x;
    }

    _rotateLeft(x) {
        const y = x.right;
        const B = y.left;
        y.left  = x;
        x.right = B;
        this._updateHeight(x);
        this._updateHeight(y);
        return y;
    }

    _balance(node) {
        this._updateHeight(node);
        const bf = this._balanceFactor(node);

        if (bf > 1) {
            if (this._balanceFactor(node.left) < 0) {
                node.left = this._rotateLeft(node.left);
            }
            return this._rotateRight(node);
        }

        if (bf < -1) {
            if (this._balanceFactor(node.right) > 0) {
                node.right = this._rotateRight(node.right);
            }
            return this._rotateLeft(node);
        }

        return node;
    }

    _insert(node, point) {
        if (!node) return new AVLNode(point);

        if (point.lessThan(node.data)) {
            node.left = this._insert(node.left, point);
        } else if (point.greaterThan(node.data)) {
            node.right = this._insert(node.right, point);
        } else {
            node.data.label = point.label;
            return node;
        }

        return this._balance(node);
    }

    insert(point) {
        this.root = this._insert(this.root, point);
    }

    _findMin(node) {
        while (node && node.left) node = node.left;
        return node;
    }

    _remove(node, label, result) {
        if (!node) return null;

        if (node.data.label === label) {
            result.found = true;

            if (!node.left || !node.right) {
                return node.left ? node.left : node.right;
            }

            const successor = this._findMin(node.right);
            node.data = { ...successor.data };
            const inner = { found: false };
            node.right = this._remove(node.right, successor.data.label, inner);
        } else {
            node.left = this._remove(node.left, label, result);
            if (!result.found) {
                node.right = this._remove(node.right, label, result);
            }
        }

        if (!node) return null;
        return this._balance(node);
    }
    remove(label) {
        const result = { found: false };
        this.root = this._remove(this.root, label, result);
        return result.found;
    }

    _inorder(node, result) {
        if (!node) return;
        this._inorder(node.left, result);
        result.push(node.data);
        this._inorder(node.right, result);
    }

    inorder() {
        const result = [];
        this._inorder(this.root, result);
        return result;
    }

    _rangeSearch(node, xMin, xMax, result) {
        if (!node) return;
        if (node.data.x >= xMin) this._rangeSearch(node.left, xMin, xMax, result);
        if (node.data.x >= xMin && node.data.x <= xMax) result.push(node.data);
        if (node.data.x <= xMax) this._rangeSearch(node.right, xMin, xMax, result);
    }

    rangeSearch(xMin, xMax) {
        const result = [];
        this._rangeSearch(this.root, xMin, xMax, result);
        return result;
    }

    _findByLabel(node, label) {
        if (!node) return null;
        if (node.data.label === label) return node.data;
        return this._findByLabel(node.left, label) ||
               this._findByLabel(node.right, label);
    }

    findByLabel(label) {
        return this._findByLabel(this.root, label);
    }

    isEmpty() {
        return this.root === null;
    }
}