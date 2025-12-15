const fs = require("fs");

function loadGraph() {
    const graph = {};
    const data = fs.readFileSync("D:/CODE/OESK/graph_with_weights.txt", "utf-8");
    const lines = data.split(/\r?\n/);

    for (const line of lines) {
        if (line.startsWith("#") || !line.trim()) continue;
        const parts = line.trim().split(/\s+/);
        if (parts.length !== 3) continue;
        let [fromNode, toNode, weight] = parts;
        fromNode = parseInt(fromNode, 10);
        toNode = parseInt(toNode, 10);
        if (!graph[fromNode]) graph[fromNode] = [];
        graph[fromNode].push(toNode);
        if (!graph[toNode]) graph[toNode] = [];
    }
    return graph;
}

function bfs(start) {
    graph = loadGraph();
    const visited = new Set();
    const queue = [start];
    visited.add(start);

    let head = 0;

    while (head < queue.length) {
        const node = queue[head++]; 
        const neighbors = graph[node];

        if (neighbors) {
            for (let i = 0; i < neighbors.length; i++) {
                const neighbor = neighbors[i];
                if (!visited.has(neighbor)) {
                    visited.add(neighbor);
                    queue.push(neighbor);
                }
            }
        }
    }
    
    return queue.slice(-5);
}

const lastNodes = bfs(0);

console.log("Last 5 nodes:", lastNodes);
console.log("koniec");