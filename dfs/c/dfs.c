#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Increased to cover your 317,080 nodes + safety buffer
// In a real production app, we would scan the file first to find the max ID, 
// but a generous constant works for this specific dataset.
#define MAX_NODES 500000 
#define BUFFER_SIZE 256
#define FILE_PATH "D:/CODE/OESK/graph_with_weights.txt"

// --- Structures ---

typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

// The Graph structure
// We use pointers here instead of fixed arrays inside the struct.
// This allows the struct itself to remain small, while pointing to massive arrays in the Heap.
typedef struct Graph {
    Node** adjLists; // Pointer to array of Node pointers
    bool* visited;   // Pointer to array of bools
} Graph;

// --- Helper Functions ---

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

    // Allocate the huge arrays on the HEAP
    // 350,000 * 8 bytes = ~2.8 MB
    graph->adjLists = malloc(MAX_NODES * sizeof(Node*));
    // 350,000 * 1 byte = ~350 KB
    graph->visited = malloc(MAX_NODES * sizeof(bool));

    if (!graph->adjLists || !graph->visited) {
        perror("Memory allocation failed for Graph arrays");
        exit(EXIT_FAILURE);
    }

    // Initialize arrays
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

// --- Core Logic ---

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

        // Critical Check: Ensure we don't segfault if data exceeds our guess
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
    
    // HEAP ALLOCATION FOR STACK
    // A local array "int stack[350000]" would crash the program (Stack Overflow).
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
            
            // Print is commented out for performance on large datasets. 
            // Printing 300k lines to console is very slow. 
            // Uncomment if you really need to see the output.
            // printf("%d \n", current_node);

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

    // --- Cleanup ---
    // In C, you clean up your own mess.
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