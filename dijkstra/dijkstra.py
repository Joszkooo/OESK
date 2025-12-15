import heapq
graph_test = {
  "A": {"B": 3, "C": 3},
  "B": {"A": 3, "D": 3.5, "E": 2.8},
  "C": {"A": 3, "E": 2.8, "F": 3.5},
  "D": {"B": 3.5, "E": 3.1, "G": 10},
  "E": {"B": 2.8, "C": 2.8, "D": 3.1, "G": 7},
  "F": {"G": 2.5, "C": 3.5},
  "G": {"F": 2.5, "E": 7, "D": 10},
}

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

infinity = float("inf")

def dijkstra_heap(G, start_node):
  shortest_paths = {} 
  visited = set()
  heap = []
  
  parents = {}

  for node in G.keys():
    shortest_paths[node] = infinity
    parents[node] = None 

  shortest_paths[start_node] = 0 
  
  heapq.heappush(heap, (0, start_node))

  while heap:
    (current_distance, current_node) = heapq.heappop(heap)
    
    if current_node in visited:
      continue
    visited.add(current_node)

    for neighbor, weight in G[current_node].items():
      distance = current_distance + weight

      if distance < shortest_paths[neighbor]:
        shortest_paths[neighbor] = distance
        parents[neighbor] = current_node 
        heapq.heappush(heap, (distance, neighbor))

  return shortest_paths, parents

def reconstruct_path(parents, end_node):
  path = []
  current = end_node
  
  while current is not None:
    path.append(current)
    current = parents[current]
  
  return path[::-1]

G = load_graph()
start = 1
end = 425875

costs, parents = dijkstra_heap(G, start)
path = reconstruct_path(parents, end)

print(f"Cost to {end}: {costs[end]}")
print(f"Path to {end}: {path}")
