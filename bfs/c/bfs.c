#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Set slightly higher than your node count to handle 0-based or 1-based indexing safety
// If your Node IDs are not contiguous (e.g. Node IDs are 999999 and 888888), 
// we would need a Hash Map. Assuming contiguous IDs (0 to ~317080) for this implementation.
#define MAX_NODES 4000000 
#define BUFFER_SIZE 1024

// --- DATA STRUCTURES ---

struct Node {
    int vertex;
    struct Node* next;
};

struct Graph {
    int numVertices;
    struct Node** adjLists; // Pointer to array of pointers (Heap allocated)
    bool* visited;          // Pointer to boolean array (Heap allocated)
};

// Queue structures
struct QueueNode {
    int vertex;
    struct QueueNode* next;
};

struct Queue {
    struct QueueNode *front, *rear;
};

// --- GRAPH MEMORY MANAGEMENT ---

struct Graph* createGraph(int vertices) {
    struct Graph* graph = malloc(sizeof(struct Graph));
    graph->numVertices = vertices;

    // ALlocate adjacency lists on the HEAP (Crucial for large datasets)
    // calloc initializes everything to NULL/0, saving us a loop
    graph->adjLists = calloc(vertices, sizeof(struct Node*));
    graph->visited = calloc(vertices, sizeof(bool));

    if (!graph->adjLists || !graph->visited) {
        perror("Failed to allocate graph memory. RAM full?");
        exit(EXIT_FAILURE);
    }

    return graph;
}

struct Node* createNode(int v) {
    // Standard malloc is fine here, but for 1M+ edges, 
    // a "Memory Pool" allocator would be the next optimization step.
    struct Node* newNode = malloc(sizeof(struct Node));
    if (!newNode) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    newNode->vertex = v;
    newNode->next = NULL;
    return newNode;
}

void addEdge(struct Graph* graph, int src, int dest) {
    struct Node* newNode = createNode(dest);
    newNode->next = graph->adjLists[src];
    graph->adjLists[src] = newNode;
}

// --- QUEUE MANAGEMENT ---

struct Queue* createQueue() {
    struct Queue* q = malloc(sizeof(struct Queue));
    q->front = q->rear = NULL;
    return q;
}

void enqueue(struct Queue* q, int value) {
    struct QueueNode* temp = malloc(sizeof(struct QueueNode));
    temp->vertex = value;
    temp->next = NULL;
    if (q->rear == NULL) {
        q->front = q->rear = temp;
        return;
    }
    q->rear->next = temp;
    q->rear = temp;
}

int dequeue(struct Queue* q) {
    if (q->front == NULL) return -1;
    struct QueueNode* temp = q->front;
    int vertex = temp->vertex;
    q->front = q->front->next;
    if (q->front == NULL) q->rear = NULL;
    free(temp);
    return vertex;
}

bool isQueueEmpty(struct Queue* q) {
    return q->front == NULL;
}

// --- FILE LOADING & BFS ---

struct Graph* load_graph(const char* filename) {
    printf("Loading graph from %s...\n", filename);
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    // Initialize graph with safe upper bound
    struct Graph* graph = createGraph(MAX_NODES);
    
    char line[BUFFER_SIZE];
    int edgesCount = 0;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        int from_node, to_node, weight;
        
        // Fast parsing
        if (sscanf(line, "%d %d %d", &from_node, &to_node, &weight) != 3) {
            continue; 
        }

        if (from_node >= MAX_NODES || to_node >= MAX_NODES) {
            // If this prints, MAX_NODES is too small for your specific IDs
            fprintf(stderr, "ID Error: Node %d or %d > %d\n", from_node, to_node, MAX_NODES);
            continue;
        }

        addEdge(graph, from_node, to_node);
        edgesCount++;
    }

    fclose(file);
    printf("Graph loaded successfully: %d Nodes (capacity), %d Edges loaded.\n", MAX_NODES, edgesCount);
    return graph;
}

void bfs(const char* filename, int startNode) {
    struct Graph* graph = load_graph(filename);
    if (!graph) return;

    if (startNode >= graph->numVertices) {
        printf("Start node %d is out of bounds (Max: %d).\n", startNode, graph->numVertices);
        return;
    }

    struct Queue* q = createQueue();
    enqueue(q, startNode);
    
    // Mark start as visited immediately to avoid re-adding
    graph->visited[startNode] = true;
    
    long long nodesVisited = 0;

    while (!isQueueEmpty(q)) {
        int currentNode = dequeue(q);
        printf("%d \n", currentNode);
        
        // Just printing the count every 10,000 nodes to keep console clean
        nodesVisited++;
        if (nodesVisited % 10000 == 0) {
            printf("Visited %lld nodes...\r", nodesVisited); // \r overwrites line
            fflush(stdout);
        }

        // --- PROCESSING NODE --- 
        
        struct Node* temp = graph->adjLists[currentNode];
        while (temp) {
            int adjVertex = temp->vertex;
            if (!graph->visited[adjVertex]) {
                graph->visited[adjVertex] = true;
                enqueue(q, adjVertex);
            }
            temp = temp->next;
        }
    }
    
    printf("\nBFS Complete. Total nodes visited: %lld\n", nodesVisited);
    
    // TODO: cleanup memory (free graph and queue)
}

int main() {
    // Ensure you use the double backslashes for Windows paths
    const char* filepath = "D:\\CODE\\OESK\\graph_with_weights.txt";
    
    // Assuming node IDs start at 0 or 1.
    bfs(filepath, 1); 

    return 0;
}