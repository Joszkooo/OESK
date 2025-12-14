using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace AStarPathfinding
{
    // Simple struct to hold connection data (lighter than a class)
    public struct Edge
    {
        public int ToNode;
        public int Weight;

        public Edge(int to, int weight)
        {
            ToNode = to;
            Weight = weight;
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Loading graph...");
            var graph = LoadGraph(@"D:\CODE\OESK\graph_with_weights.txt");

            if (graph.Count > 0)
            {
                Console.WriteLine($"Graph loaded with {graph.Count} nodes.");

                int startNode = 0;
                int goalNode = 425875;

                // Ensure nodes exist to avoid crash
                if (graph.ContainsKey(startNode) && graph.ContainsKey(goalNode))
                {
                    Console.WriteLine($"Searching for path from {startNode} to {goalNode}...");
                    
                    var watch = System.Diagnostics.Stopwatch.StartNew();
                    var path = AStar(graph, startNode, goalNode);
                    watch.Stop();

                    if (path != null)
                    {
                        Console.WriteLine("Path found: " + string.Join(" -> ", path));
                    }
                    else
                    {
                        Console.WriteLine("No path exists.");
                    }
                    Console.WriteLine($"Search took: {watch.ElapsedMilliseconds} ms");
                }
                else
                {
                    Console.WriteLine("Start or Goal node not found in graph.");
                }
            }
        }

        // --- 1. THE ALGORITHM ---

        static List<int> AStar(Dictionary<int, List<Edge>> graph, int start, int goal)
        {
            // Priority Queue: Stores <NodeID> prioritized by <F_Score>
            var openSet = new MinHeap<int>();
            openSet.Enqueue(start, 0);

            // Tracks the path
            var cameFrom = new Dictionary<int, int>();

            // Cost from start to node
            var gScore = new Dictionary<int, int>();
            // Initialize start node
            gScore[start] = 0;

            // We use a HashSet to track if a node is currently in the openSet (optimization)
            // But standard A* with lazy deletion (checking gScore) is usually enough.
            
            while (openSet.Count > 0)
            {
                // Get node with lowest F score
                var (current, currentF) = openSet.Dequeue();

                if (current == goal)
                    return ReconstructPath(cameFrom, current);

                // Optimization: Stale Node check
                // If we found a shorter path to 'current' already, skip this old entry
                int currentG;
                if (!gScore.TryGetValue(current, out currentG)) currentG = int.MaxValue;
                
                // If the F score we pulled is strictly worse than G + H, it's stale
                if (currentF > currentG + Heuristic(current, goal))
                    continue;

                // Check neighbors
                if (graph.TryGetValue(current, out List<Edge> neighbors))
                {
                    foreach (var edge in neighbors)
                    {
                        int tentativeG = currentG + edge.Weight;

                        // Check if this path is better than any previous path to neighbor
                        int neighborG;
                        if (!gScore.TryGetValue(edge.ToNode, out neighborG)) neighborG = int.MaxValue;

                        if (tentativeG < neighborG)
                        {
                            cameFrom[edge.ToNode] = current;
                            gScore[edge.ToNode] = tentativeG;

                            int fScore = tentativeG + Heuristic(edge.ToNode, goal);
                            openSet.Enqueue(edge.ToNode, fScore);
                        }
                    }
                }
            }

            return null; // Path not found
        }

        static int Heuristic(int node, int goal)
        {
            // Return 0 because we lack X,Y coordinates.
            // Behaves like Dijkstra (guaranteed shortest path).
            return 0;
        }

        static List<int> ReconstructPath(Dictionary<int, int> cameFrom, int current)
        {
            var path = new List<int> { current };
            while (cameFrom.ContainsKey(current))
            {
                current = cameFrom[current];
                path.Add(current);
            }
            path.Reverse();
            return path;
        }

        // --- 2. GRAPH LOADER ---

        static Dictionary<int, List<Edge>> LoadGraph(string filePath)
        {
            var graph = new Dictionary<int, List<Edge>>();

            if (!File.Exists(filePath))
            {
                Console.WriteLine("Error: File not found.");
                return graph;
            }

            // StreamReader is efficient for reading line by line
            using (StreamReader sr = new StreamReader(filePath))
            {
                string line;
                while ((line = sr.ReadLine()) != null)
                {
                    // Skip comments and empty lines
                    if (string.IsNullOrWhiteSpace(line) || line.StartsWith("#"))
                        continue;

                    // Efficient splitting
                    var parts = line.Split(new char[] { ' ', '\t' }, StringSplitOptions.RemoveEmptyEntries);
                    
                    if (parts.Length != 3) continue;

                    if (int.TryParse(parts[0], out int fromNode) &&
                        int.TryParse(parts[1], out int toNode) &&
                        int.TryParse(parts[2], out int weight))
                    {
                        if (!graph.ContainsKey(fromNode))
                            graph[fromNode] = new List<Edge>();

                        graph[fromNode].Add(new Edge(toNode, weight));

                        // Ensure destination node exists as a key to prevent lookup errors later
                        if (!graph.ContainsKey(toNode))
                            graph[toNode] = new List<Edge>();
                    }
                }
            }
            return graph;
        }
    }

    // --- 3. HIGH PERFORMANCE PRIORITY QUEUE ---
    // A simple binary heap implementation. 
    // This works on ALL .NET versions (Unity, Framework, Core).
    public class MinHeap<T>
    {
        private List<(T Item, int Priority)> _elements = new List<(T, int)>();

        public int Count => _elements.Count;

        public void Enqueue(T item, int priority)
        {
            _elements.Add((item, priority));
            int i = _elements.Count - 1;

            // Bubble up
            while (i > 0)
            {
                int parent = (i - 1) / 2;
                if (_elements[i].Priority >= _elements[parent].Priority)
                    break;

                var temp = _elements[i];
                _elements[i] = _elements[parent];
                _elements[parent] = temp;
                i = parent;
            }
        }

        public (T Item, int Priority) Dequeue()
        {
            var bestItem = _elements[0];
            int lastIndex = _elements.Count - 1;
            
            // Move last item to top
            _elements[0] = _elements[lastIndex];
            _elements.RemoveAt(lastIndex);

            lastIndex--;

            // Bubble down
            int i = 0;
            while (true)
            {
                int left = i * 2 + 1;
                int right = i * 2 + 2;
                int smallest = i;

                if (left <= lastIndex && _elements[left].Priority < _elements[smallest].Priority)
                    smallest = left;
                if (right <= lastIndex && _elements[right].Priority < _elements[smallest].Priority)
                    smallest = right;

                if (smallest == i) break;

                var temp = _elements[i];
                _elements[i] = _elements[smallest];
                _elements[smallest] = temp;
                i = smallest;
            }

            return bestItem;
        }
    }
}