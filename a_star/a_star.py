# funkcja do załadowania grafu
def load_graph():
  graph = {}
  # otworzenie pliku
  with open("D:/CODE/OESK/graph_with_weights.txt", "r") as file:
    for line in file:
      # jesli zaczynamy linie od # lub są puste linie to skip
      if line.startswith("#") or not line.strip():
        continue
      # dzielimy każdy node na dwie różne listy
      parts = line.split()
      # jeśli byłby błąd i pojawiłyby się więcej lub mniej niz 3 to skip
      if len(parts) != 3:
        continue
      
      from_node, to_node, weight = parts
      # zmieniamy na inty
      from_node = int(from_node)
      to_node = int(to_node)
      weight = int(weight)

      # jesli klucza nie ma w grafie to dodajemy
      if from_node not in graph:
        graph[from_node] = {}
      # kazda wartosc do klucza dodajemy
      graph[from_node][to_node] = weight
      # jeśli dziecko jest ostatnie to tworzymy je rodzicem z pustym zbiorem
      if to_node not in graph:
        graph[to_node] = {}

  return graph
import heapq

# 1. THE HEURISTIC
# Since your graph loader only has IDs and weights (no X, Y coordinates),
# we default this to 0. This makes A* behave like Dijkstra's algorithm (guaranteed shortest path).
# If you have coordinates, change this to: return abs(a.x - b.x) + abs(a.y - b.y)
def heuristic(node, goal):
    return 0

def a_star(graph, start, goal):
    # Priority Queue stores tuples of: (f_score, node)
    # f_score = g_score + h_score
    open_set = []
    heapq.heappush(open_set, (0, start))
    
    # Tracks where we came from to reconstruct the path later
    came_from = {}
    
    # g_score: The cost of the cheapest path from start to currently known node
    # Default is infinity
    g_score = {node: float('inf') for node in graph}
    g_score[start] = 0
    
    # Keep track of nodes in the heap to avoid adding duplicates if possible, 
    # though standard lazy deletion handles this efficiently.
    
    while open_set:
        # Pop the node with the lowest f_score
        current_f, current = heapq.heappop(open_set)
        
        # If we reached the goal, reconstruct and return the path
        if current == goal:
            return reconstruct_path(came_from, current)
        
        # Optimization: If the popped node has a higher g_score than what we 
        # already found, it's a "stale" entry in the heap. Skip it.
        # Note: We subtract heuristic roughly or check g_score directly if possible.
        # Typically checking g_score[current] is safer:
        if current_f > g_score.get(current, float('inf')) + heuristic(current, goal):
            continue

        # Check neighbors
        # Your graph structure is graph[node][neighbor] = weight
        if current in graph:
            for neighbor, weight in graph[current].items():
                tentative_g_score = g_score[current] + weight
                
                # If this path to neighbor is better than any previous one, record it
                if tentative_g_score < g_score.get(neighbor, float('inf')):
                    came_from[neighbor] = current
                    g_score[neighbor] = tentative_g_score
                    f_score = tentative_g_score + heuristic(neighbor, goal)
                    heapq.heappush(open_set, (f_score, neighbor))
    
    return None # Path not found

def reconstruct_path(came_from, current):
    total_path = [current]
    while current in came_from:
        current = came_from[current]
        total_path.append(current)
    return total_path[::-1] # Return reversed path (Start -> Goal)

G = load_graph()
start = 1
end = 425875

if start in G and end in G:
  print(f"Searching for path from {start} to {end}...")
  path = a_star(G, start, end)
  
  if path:
    print("Path found:", path)
  else:
    print("No path exists between these nodes.")