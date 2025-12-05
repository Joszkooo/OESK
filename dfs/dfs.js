//  Tree Structure:
//                      A
//                   /     \
//                 B         C
//               /   \     /   \
//             D      E   F     G
//            /      / \   \
//           H      J   K   L

tree = {
    'A': ['B', 'C'],
    'B': ['D', 'E'],
    'C': ['F', 'G'],
    'D': ['H'],
    'E': ['J', 'K'],
    'F': ['L'],
    'G': [],
    'H': [], 'J': [],
    'K': [], 'L': [],
}
const fs = require("fs");

function loadGraph() {
    const graph = {};

    // Read file line-by-line
    const data = fs.readFileSync("D:/CODE/OESK/graph_with_weights.txt", "utf-8");
    const lines = data.split(/\r?\n/);

    for (const line of lines) {
        // Skip comments (#...) and empty lines
        if (line.startsWith("#") || !line.trim()) {
            continue;
        }

        // Split into parts
        const parts = line.trim().split(/\s+/);

        // Skip malformed lines
        if (parts.length !== 3) {
            continue;
        }

        let [fromNode, toNode, weight] = parts;

        // Convert to integers
        fromNode = parseInt(fromNode, 10);
        toNode = parseInt(toNode, 10);

        // Add key if missing
        if (!graph[fromNode]) {
            graph[fromNode] = [];
        }

        // Append child
        graph[fromNode].push(toNode);

        // Ensure child exists as key
        if (!graph[toNode]) {
            graph[toNode] = [];
        }
    }

    return graph;
}

function dfs(start) {
  const graph = loadGraph(); 
  const visited = new Set();
  const stack = [start];

  while (stack.length > 0) {
    const node = stack.pop();
    if (!visited.has(node)) {
      visited.add(node);
      console.log(node);
      
      const children = graph[node] || [];
      for (let i = children.length - 1; i >= 0; i--) {
        stack.push(children[i]);
      }
    }
  }
}

dfs(1);
console.log("koniec");