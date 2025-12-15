using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace GraphPathfinder
{
    public class Program
    {
        const long Infinity = long.MaxValue;
        const string FilePath = @"D:\CODE\OESK\graph_with_weights.txt";

        public static void Main(string[] args)
        {
            try
            {
                var graph = LoadGraph(FilePath);
                
                int start = 0;
                int end = 425875;

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

        static Dictionary<int, Dictionary<int, int>> LoadGraph(string path)
        {
            var graph = new Dictionary<int, Dictionary<int, int>>();

            if (!File.Exists(path))
                throw new FileNotFoundException($"File not found at {path}");

            using (var reader = new StreamReader(path))
            {
                string? line;
                while ((line = reader.ReadLine()) != null)
                {
                    if (line.StartsWith("#") || string.IsNullOrWhiteSpace(line))
                        continue;

                    var parts = line.Split(new[] { ' ', '\t' }, StringSplitOptions.RemoveEmptyEntries);

                    if (parts.Length != 3)
                        continue;

                    if (int.TryParse(parts[0], out int fromNode) &&
                        int.TryParse(parts[1], out int toNode) &&
                        int.TryParse(parts[2], out int weight))
                    {
                        if (!graph.ContainsKey(fromNode))
                        {
                            graph[fromNode] = new Dictionary<int, int>();
                        }

                        graph[fromNode][toNode] = weight;

                        if (!graph.ContainsKey(toNode))
                        {
                            graph[toNode] = new Dictionary<int, int>();
                        }
                    }
                }
            }

            return graph;
        }

        static (Dictionary<int, long> costs, Dictionary<int, int?> parents) DijkstraHeap(
            Dictionary<int, Dictionary<int, int>> G, 
            int startNode)
        {
            var shortestPaths = new Dictionary<int, long>();
            var parents = new Dictionary<int, int?>();
            var visited = new HashSet<int>();

            var pq = new PriorityQueue<int, long>();

            foreach (var node in G.Keys)
            {
                shortestPaths[node] = Infinity;
                parents[node] = null;
            }

            shortestPaths[startNode] = 0;
            pq.Enqueue(startNode, 0);

            while (pq.Count > 0)
            {
                if (!pq.TryDequeue(out int currentNode, out long currentDistance))
                    break;

                if (visited.Contains(currentNode))
                    continue;

                visited.Add(currentNode);

                if (currentDistance > shortestPaths[currentNode])
                    continue;

                foreach (var edge in G[currentNode])
                {
                    int neighbor = edge.Key;
                    int weight = edge.Value;
                    long distance = currentDistance + weight;

                    if (distance < shortestPaths[neighbor])
                    {
                        shortestPaths[neighbor] = distance;
                        parents[neighbor] = currentNode;
                        
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

            if (!parents.ContainsKey(endNode))
                return path;

            while (current.HasValue)
            {
                path.Add(current.Value);
                current = parents[current.Value];
            }

            path.Reverse();
            return path;
        }
    }
}