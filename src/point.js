export class Point {
    constructor(x = 0, y = 0, label = '') {
        this.x = x;
        this.y = y;
        this.label = label;
    }

    equals(other) {
        return Math.abs(this.x - other.x) < 1e-9 &&
               Math.abs(this.y - other.y) < 1e-9;
    }

    lessThan(other) {
        if (Math.abs(this.x - other.x) < 1e-9) return this.y < other.y;
        return this.x < other.x;
    }

    greaterThan(other) {
        return other.lessThan(this);
    }

    toString() {
        return `[${this.label}] (${this.x}, ${this.y})`;
    }
}