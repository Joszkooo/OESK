#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define INF INT_MAX

typedef struct Edge {
    int to_node;
    int weight;
    struct Edge* next;
} Edge;

typedef struct {
    int num_nodes;   
    Edge** adj_lists; 
} Graph;

typedef struct {
    int node;
    int f_score;
} HeapNode;

typedef struct {
    HeapNode* data;
    int size;
    int capacity;
} MinHeap;


MinHeap* create_heap(int capacity) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    heap->data = (HeapNode*)malloc(capacity * sizeof(HeapNode));
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

void swap(HeapNode* a, HeapNode* b) {
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

void heap_push(MinHeap* heap, int node, int f_score) {
    if (heap->size == heap->capacity) return;

    int i = heap->size++;
    heap->data[i].node = node;
    heap->data[i].f_score = f_score;

    while (i != 0 && heap->data[(i - 1) / 2].f_score > heap->data[i].f_score) {
        swap(&heap->data[i], &heap->data[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

HeapNode heap_pop(MinHeap* heap) {
    if (heap->size <= 0) {
        HeapNode empty = {-1, -1};
        return empty;
    }
    if (heap->size == 1) {
        heap->size--;
        return heap->data[0];
    }

    HeapNode root = heap->data[0];
    heap->data[0] = heap->data[heap->size - 1];
    heap->size--;

    int i = 0;
    while (1) {
        int smallest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < heap->size && heap->data[left].f_score < heap->data[smallest].f_score)
            smallest = left;
        if (right < heap->size && heap->data[right].f_score < heap->data[smallest].f_score)
            smallest = right;

        if (smallest == i) break;

        swap(&heap->data[i], &heap->data[smallest]);
        i = smallest;
    }

    return root;
}


Graph* create_graph(int num_nodes) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->num_nodes = num_nodes;
    graph->adj_lists = (Edge**)calloc(num_nodes, sizeof(Edge*));
    return graph;
}

void add_edge(Graph* graph, int src, int dest, int weight) {
    Edge* new_edge = (Edge*)malloc(sizeof(Edge));
    new_edge->to_node = dest;
    new_edge->weight = weight;
    
    new_edge->next = graph->adj_lists[src];
    graph->adj_lists[src] = new_edge;
}


int heuristic(int node, int goal) {
    return 0;
}

void a_star(Graph* graph, int start, int goal) {
    int* g_score = (int*)malloc(graph->num_nodes * sizeof(int));
    int* came_from = (int*)malloc(graph->num_nodes * sizeof(int));
    
    // Initialize
    for (int i = 0; i < graph->num_nodes; i++) {
        g_score[i] = INF;
        came_from[i] = -1;
    }

    g_score[start] = 0;

    MinHeap* open_set = create_heap(graph->num_nodes); 
    heap_push(open_set, start, 0);

    int found = 0;

    while (open_set->size > 0) {
        HeapNode currentObj = heap_pop(open_set);
        int current = currentObj.node;

        if (current == goal) {
            found = 1;
            break;
        }

        if (currentObj.f_score > g_score[current] + heuristic(current, goal)) {
            continue;
        }

        // Iterate neighbors
        Edge* adj = graph->adj_lists[current];
        while (adj != NULL) {
            int neighbor = adj->to_node;
            int weight = adj->weight;

            if (g_score[current] != INF && g_score[current] + weight < g_score[neighbor]) {
                g_score[neighbor] = g_score[current] + weight;
                came_from[neighbor] = current;
                
                int f_score = g_score[neighbor] + heuristic(neighbor, goal);
                heap_push(open_set, neighbor, f_score);
            }
            adj = adj->next;
        }
    }

    if (found) {
        printf("Path found: ");
        int* path_stack = (int*)malloc(graph->num_nodes * sizeof(int));
        int path_len = 0;
        
        int curr = goal;
        while (curr != -1) {
            path_stack[path_len++] = curr;
            curr = came_from[curr];
        }

        for (int i = path_len - 1; i >= 0; i--) {
            printf("%d ", path_stack[i]);
        }
        printf("\n");
        free(path_stack);
    } else {
        printf("No path exists between %d and %d.\n", start, goal);
    }

    free(g_score);
    free(came_from);
    free(open_set->data);
    free(open_set);
}


Graph* load_graph(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return NULL;
    }

    char line[256];
    int max_node_id = 0;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        
        int u, v, w;
        if (sscanf(line, "%d %d %d", &u, &v, &w) == 3) {
            if (u > max_node_id) max_node_id = u;
            if (v > max_node_id) max_node_id = v;
        }
    }

    rewind(file);

    Graph* graph = create_graph(max_node_id + 1);

    // PASS 2: Load data
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        
        int u, v, w;
        if (sscanf(line, "%d %d %d", &u, &v, &w) == 3) {
            add_edge(graph, u, v, w);
        }
    }

    fclose(file);
    return graph;
}


int main() {
    printf("Loading graph...\n");
    Graph* my_graph = load_graph("D:/CODE/OESK/graph_with_weights.txt");

    if (my_graph) {
        printf("Graph loaded. Max Node ID: %d\n", my_graph->num_nodes - 1);

        int start_node = 0;
        int goal_node = 425875;

        if (start_node < my_graph->num_nodes && goal_node < my_graph->num_nodes) {
            printf("Searching path from %d to %d...\n", start_node, goal_node);
            a_star(my_graph, start_node, goal_node);
        } else {
            printf("Start or Goal node is out of bounds.\n");
        }

        for(int i=0; i < my_graph->num_nodes; i++) {
            Edge* curr = my_graph->adj_lists[i];
            while(curr) {
                Edge* temp = curr;
                curr = curr->next;
                free(temp);
            }
        }
        free(my_graph->adj_lists);
        free(my_graph);
    }

    return 0;
}