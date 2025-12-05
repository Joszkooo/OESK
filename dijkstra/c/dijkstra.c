#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_LINE_LEN 256
#define INF LLONG_MAX

// --- Struct Definitions ---

// Linked list node for Adjacency List
typedef struct Edge {
    int to_node;
    int weight;
    struct Edge* next;
} Edge;

// Heap node for Priority Queue
typedef struct {
    int node;
    long long cost;
} HeapNode;

// MinHeap Structure
typedef struct {
    HeapNode* array;
    int size;
    int capacity;
} MinHeap;

// --- MinHeap Implementation (Priority Queue) ---

MinHeap* createMinHeap(int capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (HeapNode*)malloc(capacity * sizeof(HeapNode));
    return minHeap;
}

void swapHeapNodes(HeapNode* a, HeapNode* b) {
    HeapNode t = *a;
    *a = *b;
    *b = t;
}

void minHeapify(MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left].cost < minHeap->array[smallest].cost)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right].cost < minHeap->array[smallest].cost)
        smallest = right;

    if (smallest != idx) {
        swapHeapNodes(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

int isEmpty(MinHeap* minHeap) {
    return minHeap->size == 0;
}

HeapNode extractMin(MinHeap* minHeap) {
    if (isEmpty(minHeap)) {
        HeapNode nullNode = {-1, -1};
        return nullNode;
    }
    
    HeapNode root = minHeap->array[0];
    HeapNode lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;
    
    minHeap->size--;
    minHeapify(minHeap, 0);
    
    return root;
}

void insertMinHeap(MinHeap* minHeap, int node, long long cost) {
    if (minHeap->size == minHeap->capacity) {
        // Simple reallocation strategy if heap fills up (doubling size)
        minHeap->capacity *= 2;
        minHeap->array = (HeapNode*)realloc(minHeap->array, minHeap->capacity * sizeof(HeapNode));
    }

    minHeap->size++;
    int i = minHeap->size - 1;
    minHeap->array[i].node = node;
    minHeap->array[i].cost = cost;

    // Bubble up
    while (i && minHeap->array[(i - 1) / 2].cost > minHeap->array[i].cost) {
        swapHeapNodes(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

// --- Graph Logic ---

void addEdge(Edge** graph, int from, int to, int weight) {
    Edge* newEdge = (Edge*)malloc(sizeof(Edge));
    newEdge->to_node = to;
    newEdge->weight = weight;
    newEdge->next = graph[from]; // Insert at head
    graph[from] = newEdge;
}

// --- Main Logic ---

int main() {
    const char* filename = "D:/CODE/OESK/graph_with_weights.txt";
    FILE* file = fopen(filename, "r");

    if (!file) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        return 1;
    }

    printf("Scanning file for graph size...\n");

    // PASS 1: Determine the maximum node ID to allocate arrays
    int max_node_id = 0;
    char line[MAX_LINE_LEN];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;
        
        int u, v, w;
        if (sscanf(line, "%d %d %d", &u, &v, &w) == 3) {
            if (u > max_node_id) max_node_id = u;
            if (v > max_node_id) max_node_id = v;
        }
    }

    // Allocate Graph (Adjacency List)
    // We use max_node_id + 1 so we can use direct indexing (index 1 is node 1)
    int num_nodes = max_node_id + 1;
    Edge** graph = (Edge**)calloc(num_nodes, sizeof(Edge*));
    
    printf("Max Node ID: %d. Loading graph...\n", max_node_id);

    // PASS 2: Load Data
    rewind(file);
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;
        
        int u, v, w;
        if (sscanf(line, "%d %d %d", &u, &v, &w) == 3) {
            addEdge(graph, u, v, w);
        }
    }
    fclose(file);

    // --- Dijkstra Implementation ---
    
    int start_node = 1;
    int end_node = 425875;

    // Output arrays
    long long* dist = (long long*)malloc(num_nodes * sizeof(long long));
    int* parents = (int*)malloc(num_nodes * sizeof(int));
    int* visited = (int*)calloc(num_nodes, sizeof(int)); // 0 = false, 1 = true

    // Initialize
    for (int i = 0; i < num_nodes; i++) {
        dist[i] = INF;
        parents[i] = -1;
    }

    dist[start_node] = 0;

    // Estimate heap capacity (start with num_nodes, it handles resizing if needed)
    MinHeap* pq = createMinHeap(num_nodes);
    insertMinHeap(pq, start_node, 0);

    printf("Running Dijkstra...\n");

    while (!isEmpty(pq)) {
        HeapNode current = extractMin(pq);
        int u = current.node;
        long long d = current.cost;

        // Optimization: Skip if already visited/finalized
        // Note: In C heap implementation (lazy deletion), we might pop same node twice.
        // We only process it if it's the first time we see it finalized, OR
        // if the popped distance matches our best known distance.
        if (d > dist[u]) continue;
        
        visited[u] = 1;

        // Iterate neighbors
        Edge* temp = graph[u];
        while (temp != NULL) {
            int v = temp->to_node;
            int weight = temp->weight;

            if (dist[u] != INF && dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                parents[v] = u;
                insertMinHeap(pq, v, dist[v]);
            }
            temp = temp->next;
        }
    }

    // --- Output & Path Reconstruction ---

    if (dist[end_node] == INF) {
        printf("No path found to %d\n", end_node);
    } else {
        printf("Cost to %d: %lld\n", end_node, dist[end_node]);

        // Reconstruct path
        // First count path length to allocate or print
        int path_len = 0;
        int curr = end_node;
        while (curr != -1) {
            path_len++;
            if (curr == start_node) break;
            curr = parents[curr];
        }

        // Collect path in reverse
        int* path = (int*)malloc(path_len * sizeof(int));
        curr = end_node;
        int idx = 0;
        while (curr != -1) {
            path[idx++] = curr;
            if (curr == start_node) break;
            curr = parents[curr];
        }

        printf("Path to %d: [", end_node);
        for (int i = path_len - 1; i >= 0; i--) {
            printf("%d%s", path[i], (i > 0) ? ", " : "");
        }
        printf("]\n");
        
        free(path);
    }

    // --- Cleanup ---
    // A true C senior developer always frees memory
    for (int i = 0; i < num_nodes; i++) {
        Edge* e = graph[i];
        while (e) {
            Edge* next = e->next;
            free(e);
            e = next;
        }
    }
    free(graph);
    free(dist);
    free(parents);
    free(visited);
    free(pq->array);
    free(pq);

    return 0;
}