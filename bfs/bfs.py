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

def bfs(start):
    graph = load_graph()
    visited = []  # lista do śledzenia odwiedzonych node'ow
    queue = deque([start])  # startujemy kolejke poprzez startowy punkt

    while queue:  # dopóki coś jest w kolejce
        node = queue.popleft()  # bierzemy pierwszy node z przodu kolejki

        if node not in visited:  # sprawdzamy czy juz odwiedzilismy
            visited.append(node)  # dodajemy node do odwiedzonych
            print(node, end=" \n")  # printujemy ten node

            # dodajemy wszystkie nieodwiedzone dzieci tego node'a do kolejki aby przez nie przejsc
            for neighbor in graph[node]:
                if neighbor not in visited:
                    queue.append(neighbor)  # dodajemy nieodwiedzone dzieci do kolejki

bfs(1)
print("koniec")