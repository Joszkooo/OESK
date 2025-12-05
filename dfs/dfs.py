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

def dfs(start):
    graph = load_graph()
    visited = set()  # Track visited nodes
    stack = [start]  # Stack for DFS

    while stack:  # Continue until stack is empty
        node = stack.pop()  # Pop a node from the stack
        if node not in visited:
            visited.add(node)  # Mark node as visited
            print(node, end=" \n")  # printujemy ten node
            stack.extend(reversed(graph[node]))  # Add child nodes to stack

dfs(1)
print("koniec")