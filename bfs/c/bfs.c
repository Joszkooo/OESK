#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NODES 4000000 
#define BUFFER_SIZE 1024

// --- DATA STRUCTURES ---

struct Node {
    int vertex;
    struct Node* next;
};

struct Graph {
    int numVertices;
    struct Node** adjLists;
    bool* visited;
};

struct QueueNode {
    int vertex;
    struct QueueNode* next;
};

struct Queue {
    struct QueueNode *front, *rear;
};


struct Graph* createGraph(int vertices) {
    struct Graph* graph = malloc(sizeof(struct Graph));
    graph->numVertices = vertices;

    graph->adjLists = calloc(vertices, sizeof(struct Node*));
    graph->visited = calloc(vertices, sizeof(bool));

    if (!graph->adjLists || !graph->visited) {
        perror("Failed to allocate graph memory. RAM full?");
        exit(EXIT_FAILURE);
    }

    return graph;
}

struct Node* createNode(int v) {
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


struct Graph* load_graph(const char* filename) {
    printf("Loading graph from %s...\n", filename);
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    struct Graph* graph = createGraph(MAX_NODES);
    
    char line[BUFFER_SIZE];
    int edgesCount = 0;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        int from_node, to_node, weight;
        
        if (sscanf(line, "%d %d %d", &from_node, &to_node, &weight) != 3) {
            continue; 
        }

        if (from_node >= MAX_NODES || to_node >= MAX_NODES) {
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
    
    graph->visited[startNode] = true;
    
    long long nodesVisited = 0;

    while (!isQueueEmpty(q)) {
        int currentNode = dequeue(q);
        printf("%d \n", currentNode);
        
        nodesVisited++;
        if (nodesVisited % 10000 == 0) {
            printf("Visited %lld nodes...\r", nodesVisited);
            fflush(stdout);
        }

        
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
    
}

int main() {
    const char* filepath = "D:\\CODE\\OESK\\graph_with_weights.txt";
    
    bfs(filepath, 0); 

    return 0;
}