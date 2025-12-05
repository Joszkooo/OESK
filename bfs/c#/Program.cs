using System;
using System.Collections.Generic;
using System.IO;

class Program
{
    // Function to load the graph from file
    static Dictionary<int, List<int>> LoadGraph()
    {
        var graph = new Dictionary<int, List<int>>();
        string path = @"D:/CODE/OESK/graph_with_weights.txt";

        foreach (var line in File.ReadLines(path))
        {
            // Skip comments (#...) and empty lines
            if (line.StartsWith("#") || string.IsNullOrWhiteSpace(line))
                continue;

            // Split into parts
            var parts = line.Split(new[] { ' ', '\t' }, StringSplitOptions.RemoveEmptyEntries);

            // Skip malformed lines
            if (parts.Length != 3)
                continue;

            // Retrieve values (weight ignored)
            int fromNode = int.Parse(parts[0]);
            int toNode = int.Parse(parts[1]);

            // Add parent node if not existing
            if (!graph.ContainsKey(fromNode))
                graph[fromNode] = new List<int>();

            // Add child
            graph[fromNode].Add(toNode);

            // Ensure child also exists as a key
            if (!graph.ContainsKey(toNode))
                graph[toNode] = new List<int>();
        }

        return graph;
    }

    // BFS function
    static void BFS(int start)
    {
        var graph = LoadGraph();
        var visited = new HashSet<int>();
        var queue = new Queue<int>();

        queue.Enqueue(start);

        while (queue.Count > 0)
        {
            int node = queue.Dequeue();

            if (!visited.Contains(node))
            {
                visited.Add(node);
                Console.Write(node + "\n");

                // Add neighbors
                foreach (var neighbor in graph[node])
                {
                    if (!visited.Contains(neighbor))
                        queue.Enqueue(neighbor);
                }
            }
        }
    }

    static void Main()
    {
        BFS(1);
        Console.WriteLine("\nkoniec");
    }
}
