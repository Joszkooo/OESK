using System;
using System.Collections.Generic;
//Tree Structure:
//
//                     A
//                  /     \
//                B         C
//              /   \     /   \
//            D      E   F     G
//           /      / \   \
//          H      J   K   L
//
class Program
{
    static void Main()
    {
        DFS(1);
        Console.WriteLine("\nkoniec");
    }
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

    static void DFS(int start)
    {
        var graph = LoadGraph();
        var visited = new HashSet<int>();
        var stack = new Stack<int>();

        stack.Push(start);

        while (stack.Count > 0)
        {
            var node = stack.Pop();

            if (!visited.Contains(node))
            {
                visited.Add(node);
                Console.WriteLine(node);

                var children = graph[node];
                for (int i = children.Count - 1; i >= 0; i--)
                {
                    stack.Push(children[i]);
                }
            }
        }
    }
}
