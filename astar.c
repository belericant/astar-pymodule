#include "heap.h"
#include "astar.h"
#include <math.h>
#include <stdlib.h>

double heuristic(int x, int y, int goal_x, int goal_y) {
    int dx = abs(x - goal_x);
    int dy = abs(y - goal_y);
    return (dx + dy) + (1.414 - 2) * (dx < dy ? dx : dy); // octile distance
}

int comparator(const void *ptr1, const void *ptr2) {
    double res = ((vertex_s_t *) ptr1)->f - ((vertex_s_t *) ptr2)->f;
    return (res > 0) - (res < 0);
}

inline point_s_t coord_to_index(graph_s_t *graph, int x, int y) {
    return (point_s_t) {graph->size - y - 1, x};
}

inline point_s_t index_to_coord(graph_s_t *graph, int i, int j) {
    return (point_s_t) {j, graph->size - i - 1};
}

static inline void add_obstacle_(graph_s_t *graph, point_s_t coord) {
    graph->arr[coord.x][coord.y].deleted = 1;
}

static inline void remove_obstacle_(graph_s_t *graph, point_s_t coord) {
    graph->arr[coord.x][coord.y].deleted = 1;
}

static void modify_weights_(vertex_s_t *v1, vertex_s_t *v2, double weight) {
    for (int i = 0; i < 8; i++) {
        if (v1->edges[i].vertex == v2) {
            v1->edges[i].weight = weight;
        }
        if (v2->edges[i].vertex == v1) {
            v2->edges[i].weight = weight;
        }
    }
}

void add_obstacle(graph_s_t *graph, int x, int y) {
    add_obstacle_(graph, coord_to_index(graph, x, y));
}

void remove_obstacle(graph_s_t *graph, int x, int y) {
    remove_obstacle_(graph, coord_to_index(graph, x, y));
}

void modify_weights(graph_s_t *graph, int x1, int y1, int x2, int y2, double weight) {
    point_s_t p1 = coord_to_index(graph, x1, y1);
    point_s_t p2 = coord_to_index(graph, x2, y2);
    modify_weights_(&graph->arr[p1.x][p1.y], &graph->arr[p2.x][p2.y], weight);
}

int astar(graph_s_t *graph, vertex_s_t *start, vertex_s_t *end, double (*h)(int, int, int, int)) {
    // clear data structures
    heap_s_t *heap = heap_new(graph->size * graph->size, comparator);
    graph_init(graph);

    // add start to open set
    heap_push_back(heap, start);
    start->g = 0;

    // while there still are unvisited vertices
    while (heap->idx != 0) {
        vertex_s_t *current = heap_pop_min(heap);
        if (current == end) {
            heap_free(heap);
            return 1;
        }
        // for all neighbors of current
        for (int i = 0; i < 8; ++i) {
            vertex_s_t *neighbor = current->edges[i].vertex;
            if (neighbor && !neighbor->deleted) {
                double gScore = current->g + current->edges[i].weight;
                if (gScore < neighbor->g) {
                    neighbor->g = gScore;
                    neighbor->f = neighbor->g + h(neighbor->coord.x, neighbor->coord.y, end->coord.x, end->coord.y);
                    neighbor->parent = current;
                    if (!heap_contains(heap, neighbor)) {
                        heap_push_back(heap, neighbor);
                    } else {
                        heap_decrease_key(heap, neighbor);
                    }
                }
            }
        }
    }
    heap_free(heap);
    return 0;
}
