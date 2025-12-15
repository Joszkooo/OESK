const fs = require('fs');
const readline = require('readline');


class MinHeap {
    constructor() {
        this.heap = [];
    }

    push(val) {
        this.heap.push(val);
        this._bubbleUp();
    }

    pop() {
        if (this.size() === 0) return null;
        if (this.size() === 1) return this.heap.pop();
        
        const min = this.heap[0];
        this.heap[0] = this.heap.pop();
        this._sinkDown();
        return min;
    }

    size() {
        return this.heap.length;
    }

    _bubbleUp() {
        let index = this.heap.length - 1;
        while (index > 0) {
            const parentIndex = Math.floor((index - 1) / 2);
            if (this.heap[index][0] >= this.heap[parentIndex][0]) break;
            
            [this.heap[index], this.heap[parentIndex]] = [this.heap[parentIndex], this.heap[index]];
            index = parentIndex;
        }
    }

    _sinkDown() {
        let index = 0;
        const length = this.heap.length;
        
        while (true) {
            let leftChildIdx = 2 * index + 1;
            let rightChildIdx = 2 * index + 2;
            let swap = null;

            if (leftChildIdx < length) {
                if (this.heap[leftChildIdx][0] < this.heap[index][0]) {
                    swap = leftChildIdx;
                }
            }

            if (rightChildIdx < length) {
                if (
                    (swap === null && this.heap[rightChildIdx][0] < this.heap[index][0]) ||
                    (swap !== null && this.heap[rightChildIdx][0] < this.heap[swap][0])
                ) {
                    swap = rightChildIdx;
                }
            }

            if (swap === null) break;

            [this.heap[index], this.heap[swap]] = [this.heap[swap], this.heap[index]];
            index = swap;
        }
    }
}


const INFINITY = Infinity;

async function loadGraph() {
    const graph = {};

    const fileStream = fs.createReadStream("D:/CODE/OESK/graph_with_weights.txt");

    const rl = readline.createInterface({
        input: fileStream,
        crlfDelay: Infinity
    });

    console.log("Loading graph...");

    for await (const line of rl) {
        const trimmedLine = line.trim();
        
        if (trimmedLine.startsWith("#") || !trimmedLine) {
            continue;
        }
        const parts = trimmedLine.split(/\s+/);

        if (parts.length !== 3) {
            continue;
        }

        const fromNode = Number(parts[0]);
        const toNode = Number(parts[1]);
        const weight = Number(parts[2]);

        if (!graph[fromNode]) graph[fromNode] = {};
        if (!graph[toNode]) graph[toNode] = {}; 

        graph[fromNode][toNode] = weight;
    }

    console.log("Graph loaded.");
    return graph;
}

function dijkstraHeap(G, startNode) {
    const shortestPaths = {};
    const visited = new Set();
    const parents = {};
    
    const heap = new MinHeap();

    for (const node of Object.keys(G)) {
        shortestPaths[node] = INFINITY;
        parents[node] = null;
    }

    shortestPaths[startNode] = 0;
    
    // Heap stores [distance, node]
    heap.push([0, startNode]);

    while (heap.size() > 0) {
        const [currentDistance, currentNode] = heap.pop();

        if (visited.has(Number(currentNode))) {
            continue;
        }
        visited.add(Number(currentNode));

        const neighbors = G[currentNode];
        
        if (neighbors) {
            for (const [neighborStr, weight] of Object.entries(neighbors)) {
                const neighbor = Number(neighborStr);
                const distance = currentDistance + weight;

                if (distance < shortestPaths[neighbor]) {
                    shortestPaths[neighbor] = distance;
                    parents[neighbor] = currentNode;
                    heap.push([distance, neighbor]);
                }
            }
        }
    }

    return { costs: shortestPaths, parents };
}

function reconstructPath(parents, endNode) {
    const path = [];
    let current = endNode;

    if (parents[current] === undefined && current !== undefined) {
        return [];
    }

    while (current !== null && current !== undefined) {
        path.push(current);
        current = parents[current];
    }

    return path.reverse();
}

(async () => {
    try {
        const G = await loadGraph();
        
        const start = 1;
        const end = 425875;

        if (!G[start]) {
            console.error(`Start node ${start} not found in graph.`);
            return;
        }

        console.log(`Running Dijkstra from ${start} to ${end}...`);
        const { costs, parents } = dijkstraHeap(G, start);
        
        const path = reconstructPath(parents, end);

        if (costs[end] === INFINITY) {
            console.log(`No path found between ${start} and ${end}.`);
        } else {
            console.log(`Cost to ${end}: ${costs[end]}`);
            console.log(`Path to ${end}:`, path);
        }

    } catch (error) {
        console.error("An error occurred:", error);
    }
})();