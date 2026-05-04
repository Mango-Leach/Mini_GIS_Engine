export class MinHeap {
    constructor() {
        this.heap = []; 
    }

    _parent(i)     { return Math.floor((i - 1) / 2); }
    _leftChild(i)  { return 2 * i + 1; }
    _rightChild(i) { return 2 * i + 2; }

    _swap(i, j) {
        [this.heap[i], this.heap[j]] = [this.heap[j], this.heap[i]];
    }

    _heapifyUp(index) {
        while (index > 0 &&
               this.heap[this._parent(index)].distance > this.heap[index].distance) {
            this._swap(this._parent(index), index);
            index = this._parent(index);
        }
    }

    _heapifyDown(index) {
        let smallest = index;
        const left   = this._leftChild(index);
        const right  = this._rightChild(index);
        const n      = this.heap.length;

        if (left  < n && this.heap[left].distance  < this.heap[smallest].distance) smallest = left;
        if (right < n && this.heap[right].distance < this.heap[smallest].distance) smallest = right;

        if (smallest !== index) {
            this._swap(index, smallest);
            this._heapifyDown(smallest);
        }
    }

    insert(distance, point) {
        this.heap.push({ distance, point });
        this._heapifyUp(this.heap.length - 1);
    }

    extractMin() {
        if (this.heap.length === 0) throw new Error('MinHeap is empty');
        const min = this.heap[0];
        this.heap[0] = this.heap[this.heap.length - 1];
        this.heap.pop();
        if (this.heap.length > 0) this._heapifyDown(0);
        return min;
    }

    peek() {
        if (this.heap.length === 0) throw new Error('MinHeap is empty');
        return this.heap[0];
    }

    get size()    { return this.heap.length; }
    get empty()   { return this.heap.length === 0; }
}