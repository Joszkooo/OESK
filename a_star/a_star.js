const fs = require('fs');
const readline = require('readline');

// --- 1. FAST PRIORITY QUEUE (MinHeap) ---
// JavaScript doesn't have this built-in, so we need it for O(log n) speed.
class MinHeap {
    constructor() {
        this.heap = [];
    }

    push(val) {
        this.heap.push(val);
        this.bubbleUp(this.heap.length - 1);
    }

    pop() {
        if (this.heap.length === 0) return null;
        const min = this.heap[0];
        const end = this.heap.pop();
        if (this.heap.length > 0) {
            this.heap[0] = end;
            this.bubbleDown(0);
        }
        return min;
    }

    size() {
        return this.heap.length;
    }

    // Helper: Move element up to correct position
    bubbleUp(n) {
        while (n > 0) {
            let parent = Math.floor((n - 1) / 2);
            if (this.heap[n].f >= this.heap[parent].f) break;
            [this.heap[n], this.heap[parent]] = [this.heap[parent], this.heap[n]];
            n = parent;
        }
    }

    // Helper: Move element down to correct position
    bubbleDown(n) {
        let length = this.heap.length;
        while (true) {
            let left = 2 * n + 1;
            let right = 2 * n + 2;
            let smallest = n;

            if (left < length && this.heap[left].f < this.heap[smallest].f) smallest = left;
            if (right < length && this.heap[right].f < this.heap[smallest].f) smallest = right;
            
            if (smallest === n) break;
            
            [this.heap[n], this.heap[smallest]] = [this.heap[smallest], this.heap[n]];
            n = smallest;
        }
    }
}

// --- 2. HEURISTIC & A* ALGORITHM ---

function heuristic(node, goal) {
    // Return 0 because we don't have X,Y coordinates. 
    // Behaves like Dijkstra (guaranteed shortest path).
    return 0; 
}

function reconstructPath(cameFrom, current) {
    const totalPath = [current];
    while (cameFrom[current] !== undefined) {
        current = cameFrom[current];
        totalPath.push(current);
    }
    return totalPath.reverse();
}

function aStar(graph, start, goal) {
    // Priority Queue stores objects: { f: cost, node: id }
    const openSet = new MinHeap();
    openSet.push({ f: 0, node: start });

    const cameFrom = {}; // Map of navigation (for path reconstruction)
    
    // gScore: cost from start to node. Default is Infinity.
    const gScore = {}; 
    gScore[start] = 0;

    // Use a Set to keep track of nodes in the openSet for faster lookup
    // (Optional optimization, but helps avoid duplicates conceptually)
    
    while (openSet.size() > 0) {
        // Get node with lowest f score
        const { f: currentF, node: current } = openSet.pop();

        // If goal reached
        if (current === goal) {
            return reconstructPath(cameFrom, current);
        }

        // Lazy deletion logic:
        // If we extracted a node with a higher f cost than we already found, skip it.
        // (We check gScore vs currentF mostly, but strictly gScore lookup is safer)
        const currentG = gScore[current] !== undefined ? gScore[current] : Infinity;
        if (currentF > currentG + heuristic(current, goal)) {
            continue;
        }

        // Neighbors check
        const neighbors = graph[current];
        if (!neighbors) continue;

        for (const neighborStr in neighbors) {
            const neighbor = parseInt(neighborStr); // JSON keys are strings
            const weight = neighbors[neighbor];
            
            const tentativeGScore = currentG + weight;
            const neighborG = gScore[neighbor] !== undefined ? gScore[neighbor] : Infinity;

            if (tentativeGScore < neighborG) {
                // This path is better than any previous one
                cameFrom[neighbor] = current;
                gScore[neighbor] = tentativeGScore;
                
                const fScore = tentativeGScore + heuristic(neighbor, goal);
                openSet.push({ f: fScore, node: neighbor });
            }
        }
    }

    return null; // Path not found
}

async function loadGraph() {
    const graph = {};

    // Create a read stream to handle large files line-by-line
    const fileStream = fs.createReadStream("D:/CODE/OESK/graph_with_weights.txt");

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

// --- EXECUTION ---
(async () => {
    const myGraph = await loadGraph();
    const nodeCount = Object.keys(myGraph).length;

    if (nodeCount > 0) {
        console.log(`Graph loaded with ${nodeCount} nodes.`);

        const startNode = 0;
        const goalNode = 425876;

        // Check existence
        if (myGraph[startNode] && myGraph[goalNode]) {
            console.time("A* Search Time"); // Start timer
            const path = aStar(myGraph, startNode, goalNode);
            console.timeEnd("A* Search Time"); // Stop timer

            if (path) {
                console.log("Path found:", path);
            } else {
                console.log("No path exists.");
            }
        } else {
            console.log("Start or Goal node not in graph.");
        }
    }
})();