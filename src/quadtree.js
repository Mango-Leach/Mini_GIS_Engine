import { Distance } from './distance.js';

export class AABB {
    constructor(cx, cy, halfWidth, halfHeight) {
        this.cx         = cx;
        this.cy         = cy;
        this.halfWidth  = halfWidth;
        this.halfHeight = halfHeight;
    }

    contains(point) {
        return point.x >= this.cx - this.halfWidth  &&
               point.x <= this.cx + this.halfWidth  &&
               point.y >= this.cy - this.halfHeight &&
               point.y <= this.cy + this.halfHeight;
    }

    intersects(other) {
        return !(other.cx - other.halfWidth  > this.cx + this.halfWidth  ||
                 other.cx + other.halfWidth  < this.cx - this.halfWidth  ||
                 other.cy - other.halfHeight > this.cy + this.halfHeight ||
                 other.cy + other.halfHeight < this.cy - this.halfHeight);
    }
}

export class QuadTree {
    constructor(boundary, capacity = 4) {
        this.boundary = boundary;
        this.capacity = capacity;
        this.points   = [];
        this.divided  = false;
        this.NW = null;
        this.NE = null;
        this.SW = null;
        this.SE = null;
    }

    _subdivide() {
        const { cx, cy, halfWidth, halfHeight } = this.boundary;
        const hw = halfWidth  / 2;
        const hh = halfHeight / 2;

        this.NW = new QuadTree(new AABB(cx - hw, cy + hh, hw, hh), this.capacity);
        this.NE = new QuadTree(new AABB(cx + hw, cy + hh, hw, hh), this.capacity);
        this.SW = new QuadTree(new AABB(cx - hw, cy - hh, hw, hh), this.capacity);
        this.SE = new QuadTree(new AABB(cx + hw, cy - hh, hw, hh), this.capacity);

        this.divided = true;
    }

    insert(point) {
        if (!this.boundary.contains(point)) return false;

        if (this.points.length < this.capacity) {
            this.points.push(point);
            return true;
        }

        if (!this.divided) this._subdivide();

        return this.NW.insert(point) ||
               this.NE.insert(point) ||
               this.SW.insert(point) ||
               this.SE.insert(point);
    }

    queryRange(range, found = []) {
        if (!this.boundary.intersects(range)) return found;

        for (const p of this.points) {
            if (range.contains(p)) found.push(p);
        }

        if (this.divided) {
            this.NW.queryRange(range, found);
            this.NE.queryRange(range, found);
            this.SW.queryRange(range, found);
            this.SE.queryRange(range, found);
        }

        return found;
    }

    queryRadius(center, radius, found = []) {
        const rangeBB = new AABB(center.x, center.y, radius, radius);
        if (!this.boundary.intersects(rangeBB)) return found;

        for (const p of this.points) {
            if (Distance.euclidean(center, p) <= radius) found.push(p);
        }

        if (this.divided) {
            this.NW.queryRadius(center, radius, found);
            this.NE.queryRadius(center, radius, found);
            this.SW.queryRadius(center, radius, found);
            this.SE.queryRadius(center, radius, found);
        }

        return found;
    }

    getAllPoints(allPoints = []) {
        for (const p of this.points) allPoints.push(p);

        if (this.divided) {
            this.NW.getAllPoints(allPoints);
            this.NE.getAllPoints(allPoints);
            this.SW.getAllPoints(allPoints);
            this.SE.getAllPoints(allPoints);
        }

        return allPoints;
    }

    size() {
        let count = this.points.length;
        if (this.divided) {
            count += this.NW.size() + this.NE.size() +
                     this.SW.size() + this.SE.size();
        }
        return count;
    }

    getBoundaries(result = []) {
        result.push(this.boundary);
        if (this.divided) {
            this.NW.getBoundaries(result);
            this.NE.getBoundaries(result);
            this.SW.getBoundaries(result);
            this.SE.getBoundaries(result);
        }
        return result;
    }
}