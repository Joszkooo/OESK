using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace GraphPathfinder
{
    public class Program
    {
        // Use long for distance to prevent integer overflow on large graphs
        const long Infinity = long.MaxValue;
        const string FilePath = @"D:\CODE\OESK\graph_with_weights.txt";

        public static void Main(string[] args)
        {
            try
            {
                var graph = LoadGraph(FilePath);
                
                int start = 1;
                int end = 425875;

                // Ensure the start node actually exists to avoid runtime keys errors
                if (!graph.ContainsKey(start))
                {
                    Console.WriteLine($"Error: Start node {start} not found in graph.");
                    return;
                }

                Console.WriteLine($"Running Dijkstra from {start} to {end}...");
                
                var (costs, parents) = DijkstraHeap(graph, start);
                var path = ReconstructPath(parents, end);

                // Output results
                if (costs.ContainsKey(end) && costs[end] != Infinity)
                {
                    Console.WriteLine($"Cost to {end}: {costs[end]}");
                    Console.WriteLine($"Path to {end}: [{string.Join(", ", path)}]");
                }
                else
                {
                    Console.WriteLine($"No path found to {end}");
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"An error occurred: {ex.Message}");
            }
        }

        /// <summary>
        /// Loads the graph from a file into an adjacency list (Nested Dictionary).
        /// </summary>
        static Dictionary<int, Dictionary<int, int>> LoadGraph(string path)
        {
            var graph = new Dictionary<int, Dictionary<int, int>>();

            if (!File.Exists(path))
                throw new FileNotFoundException($"File not found at {path}");

            // StreamReader allows processing line-by-line without loading the whole file into RAM
            using (var reader = new StreamReader(path))
            {
                string? line;
                while ((line = reader.ReadLine()) != null)
                {
                    // Skip comments or empty lines
                    if (line.StartsWith("#") || string.IsNullOrWhiteSpace(line))
                        continue;

                    // Split by whitespace
                    // StringSplitOptions.RemoveEmptyEntries handles multiple spaces gracefully
                    var parts = line.Split(new[] { ' ', '\t' }, StringSplitOptions.RemoveEmptyEntries);

                    if (parts.Length != 3)
                        continue;

                    if (int.TryParse(parts[0], out int fromNode) &&
                        int.TryParse(parts[1], out int toNode) &&
                        int.TryParse(parts[2], out int weight))
                    {
                        // Initialize outer dictionary key if missing
                        if (!graph.ContainsKey(fromNode))
                        {
                            graph[fromNode] = new Dictionary<int, int>();
                        }

                        // Add edge
                        graph[fromNode][toNode] = weight;

                        // Ensure target node exists in the graph keys (even if it has no children)
                        // This mimics the Python "if to_node not in graph" logic
                        if (!graph.ContainsKey(toNode))
                        {
                            graph[toNode] = new Dictionary<int, int>();
                        }
                    }
                }
            }

            return graph;
        }

        /// <summary>
        /// Dijkstra's Algorithm using .NET 6 PriorityQueue
        /// </summary>
        static (Dictionary<int, long> costs, Dictionary<int, int?> parents) DijkstraHeap(
            Dictionary<int, Dictionary<int, int>> G, 
            int startNode)
        {
            var shortestPaths = new Dictionary<int, long>();
            var parents = new Dictionary<int, int?>();
            var visited = new HashSet<int>();

            // PriorityQueue<TElement, TPriority>
            // Element = Node ID (int), Priority = Distance (long)
            var pq = new PriorityQueue<int, long>();

            // Initialize all nodes
            foreach (var node in G.Keys)
            {
                shortestPaths[node] = Infinity;
                parents[node] = null;
            }

            // Set start node
            shortestPaths[startNode] = 0;
            pq.Enqueue(startNode, 0);

            while (pq.Count > 0)
            {
                // TryDequeue is cleaner than Dequeue for checking success, 
                // though usually Count > 0 check is enough.
                if (!pq.TryDequeue(out int currentNode, out long currentDistance))
                    break;

                // Optimization: skip if we already processed this node
                // Note: In strict Dijkstra with a generic Heap, we might pop the same node 
                // multiple times with different costs. We only want the first (smallest) one.
                if (visited.Contains(currentNode))
                    continue;

                visited.Add(currentNode);

                // If the current distance is already worse than what we found previously, skip
                // (This handles "lazy deletion" cases in Heaps)
                if (currentDistance > shortestPaths[currentNode])
                    continue;

                // Iterate neighbors
                // G[currentNode] gives us the Dictionary of neighbors
                foreach (var edge in G[currentNode])
                {
                    int neighbor = edge.Key;
                    int weight = edge.Value;
                    long distance = currentDistance + weight;

                    // If we found a shorter path
                    if (distance < shortestPaths[neighbor])
                    {
                        shortestPaths[neighbor] = distance;
                        parents[neighbor] = currentNode;
                        
                        // Enqueue with new priority
                        pq.Enqueue(neighbor, distance);
                    }
                }
            }

            return (shortestPaths, parents);
        }

        static List<int> ReconstructPath(Dictionary<int, int?> parents, int endNode)
        {
            var path = new List<int>();
            int? current = endNode;

            // Check if endNode exists in parents map to avoid endless loops or errors
            // if the node was never reached
            if (!parents.ContainsKey(endNode))
                return path; // Return empty path

            while (current.HasValue)
            {
                path.Add(current.Value);
                current = parents[current.Value];
            }

            // Reverse to get Start -> End
            path.Reverse();
            return path;
        }
    }
}