const fs = require('fs');
const readline = require('readline');
const path = require('path');

/**
 * A custom MinHeap implementation since JS lacks a built-in PriorityQueue.
 * Essential for Dijkstra's performance.
 */
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
            // Compare based on distance (first element of tuple)
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

// Configuration
const FILE_PATH = "D:/CODE/OESK/graph_with_weights.txt";
const INFINITY = Infinity;

/**
 * Loads graph from file using streams for memory efficiency.
 */
async function loadGraph(filePath) {
    const graph = {};

    // Create a read stream to handle large files line-by-line
    const fileStream = fs.createReadStream(filePath);

    const rl = readline.createInterface({
        input: fileStream,
        crlfDelay: Infinity
    });

    console.log("Loading graph...");

    for await (const line of rl) {
        const trimmedLine = line.trim();
        
        // Skip comments and empty lines
        if (trimmedLine.startsWith("#") || !trimmedLine) {
            continue;
        }

        // Split by whitespace
        const parts = trimmedLine.split(/\s+/);

        if (parts.length !== 3) {
            continue;
        }

        const fromNode = Number(parts[0]);
        const toNode = Number(parts[1]);
        const weight = Number(parts[2]);

        // Initialize objects if they don't exist
        if (!graph[fromNode]) graph[fromNode] = {};
        if (!graph[toNode]) graph[toNode] = {}; // Ensure 'to' node exists in graph keys

        graph[fromNode][toNode] = weight;
    }

    console.log("Graph loaded.");
    return graph;
}

function dijkstraHeap(G, startNode) {
    const shortestPaths = {};
    const visited = new Set();
    const parents = {};
    
    // Using our custom MinHeap
    const heap = new MinHeap();

    // Initialize
    // Note: iterating Object.keys converts keys to strings, we must use them carefully
    for (const node of Object.keys(G)) {
        shortestPaths[node] = INFINITY;
        parents[node] = null;
    }

    shortestPaths[startNode] = 0;
    
    // Heap stores [distance, node]
    heap.push([0, startNode]);

    while (heap.size() > 0) {
        const [currentDistance, currentNode] = heap.pop();

        // Skip if visited
        // Note: Object keys are strings, but we pushed numbers. 
        // Set.has is strict, so we maintain type consistency.
        if (visited.has(Number(currentNode))) {
            continue;
        }
        visited.add(Number(currentNode));

        const neighbors = G[currentNode];
        
        // Loop through neighbors
        if (neighbors) {
            for (const [neighborStr, weight] of Object.entries(neighbors)) {
                const neighbor = Number(neighborStr); // Convert back to number
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

    // Check if endNode was actually reached (it might be disconnected)
    if (parents[current] === undefined && current !== undefined) {
         // handle edge case where node doesn't exist in parents
         return [];
    }

    while (current !== null && current !== undefined) {
        path.push(current);
        current = parents[current];
    }

    // Reverse to get Start -> End
    return path.reverse();
}

// --- Main Execution ---
(async () => {
    try {
        const G = await loadGraph(FILE_PATH);
        
        const start = 1;
        const end = 425875;

        // Ensure start node exists in graph to prevent crashes
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
            
            // Javascript Arrays print nicely, but for large arrays, 
            // node might truncate the output.
            console.log(`Path to ${end}:`, path);
        }

    } catch (error) {
        console.error("An error occurred:", error);
    }
})();