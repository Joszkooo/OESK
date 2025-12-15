# Tree Structure:
#
#                     A
#                  /     \
#                B         C
#              /   \     /   \
#            D      E   F     G
#           /      / \   \
#          H      J   K   L
#

tree_test = {
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
            #bierzemy tutaj 3 rzeczy ale uzywamy tylko 2
            from_node, to_node, weight = parts
            # zmieniamy na inty
            from_node = int(from_node)
            to_node = int(to_node)

            # jesli klucza nie ma w grafie to dodajemy
            if from_node not in graph:
                graph[from_node] = []
            # kazda wartosc do klucza dodajemy
            graph[from_node].append(to_node)
            # jeśli dziecko jest ostatnie to tworzymy je rodzicem z pustym zbiorem
            if to_node not in graph:
                graph[to_node] = []

    return graph

from collections import deque

def bfs(start_node):
    graph = load_graph()

    visited = set()
    visited.add(start_node)
    
    queue = deque([start_node])
    
    # 2. Store order of visited nodes
    traversal_order = []

    while queue:
        current_node = queue.popleft()
        traversal_order.append(current_node)

        # 3. Direct access & mark-on-push
        for neighbor in graph[current_node]:
            if neighbor not in visited:
                visited.add(neighbor)
                queue.append(neighbor)
                
    return traversal_order

result = bfs(0)
print(f"Total nodes visited: {len(result)}")
print(f"Last 5 nodes: {result[-5:]}")
print("koniec")