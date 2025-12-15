#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NODES 500000 
#define BUFFER_SIZE 256
#define FILE_PATH "D:/CODE/OESK/graph_with_weights.txt"

// --- Structures ---

typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

typedef struct Graph {
    Node** adjLists;
    bool* visited;
} Graph;


Node* create_node(int v) {
    Node* newNode = malloc(sizeof(Node));
    if (!newNode) {
        perror("Memory allocation failed for Node");
        exit(EXIT_FAILURE);
    }
    newNode->vertex = v;
    newNode->next = NULL;
    return newNode;
}

Graph* create_graph() {
    Graph* graph = malloc(sizeof(Graph));
    if (!graph) {
        perror("Memory allocation failed for Graph struct");
        exit(EXIT_FAILURE);
    }

    graph->adjLists = malloc(MAX_NODES * sizeof(Node*));

    graph->visited = malloc(MAX_NODES * sizeof(bool));

    if (!graph->adjLists || !graph->visited) {
        perror("Memory allocation failed for Graph arrays");
        exit(EXIT_FAILURE);
    }


    for (int i = 0; i < MAX_NODES; i++) {
        graph->adjLists[i] = NULL;
        graph->visited[i] = false;
    }
    return graph;
}

void add_edge(Graph* graph, int src, int dest) {
    Node* newNode = create_node(dest);
    newNode->next = graph->adjLists[src];
    graph->adjLists[src] = newNode;
}

Graph* load_graph() {
    printf("Loading graph from file...\n");
    FILE* file = fopen(FILE_PATH, "r");
    if (!file) {
        perror("Error opening file. Check path");
        exit(EXIT_FAILURE);
    }

    Graph* graph = create_graph();
    char line[BUFFER_SIZE];
    int edges_loaded = 0;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }

        int from_node, to_node, weight;
        
        if (sscanf(line, "%d %d %d", &from_node, &to_node, &weight) != 3) {
            continue; 
        }

        if (from_node >= MAX_NODES || to_node >= MAX_NODES) {
            fprintf(stderr, "CRITICAL ERROR: Node ID %d or %d exceeds MAX_NODES (%d). Increase MAX_NODES.\n", from_node, to_node, MAX_NODES);
            exit(EXIT_FAILURE);
        }

        add_edge(graph, from_node, to_node);
        edges_loaded++;
    }

    printf("Graph loaded successfully. Edges: %d\n", edges_loaded);
    fclose(file);
    return graph;
}

void dfs(int start_node) {
    Graph* graph = load_graph();
    
    int* stack = malloc(MAX_NODES * sizeof(int));
    if (!stack) {
        perror("Memory allocation failed for DFS stack");
        exit(EXIT_FAILURE);
    }

    int top = -1;
    stack[++top] = start_node;

    printf("Starting DFS...\n");

    while (top >= 0) {
        int current_node = stack[top--]; 

        if (!graph->visited[current_node]) {
            graph->visited[current_node] = true;
            

            Node* temp = graph->adjLists[current_node];
            while (temp) {
                if (!graph->visited[temp->vertex]) {
                    stack[++top] = temp->vertex;
                }
                temp = temp->next;
            }
        }
    }

    printf("DFS Completed.\n");

    free(stack);
    for (int i = 0; i < MAX_NODES; i++) {
        Node* temp = graph->adjLists[i];
        while (temp) {
            Node* toFree = temp;
            temp = temp->next;
            free(toFree);
        }
    }
    free(graph->adjLists);
    free(graph->visited);
    free(graph);
}

int main() {
    dfs(1);
    printf("koniec\n");
    return 0;
}