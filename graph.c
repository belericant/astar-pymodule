#include "graph.h"
#include <stdlib.h>
#include <float.h>
#include <string.h>

void set_weights(graph_s_t *graph, int i, int j, adjacent_vertex_e_t adj) {
    vertex_s_t *vertex = &graph->arr[i][j];
    switch (adj) {
        case W:
            if (j == 0) { // if the vertex is on the left column
                return;
            }
            vertex->edges[W].vertex = &graph->arr[i][j - 1];
            vertex->edges[W].weight = 1;
            break;

        case E:
            if (j == graph->size) { // if the vertex is on the right column
                return;
            }
            vertex->edges[E].vertex = &graph->arr[i][j + 1];
            vertex->edges[E].weight = 1;
            break;

        case N:
            if (i == 0) { // if the vertex is on the top row
                return;
            }
            vertex->edges[N].vertex = &graph->arr[i - 1][j];
            vertex->edges[N].weight = 1;
            break;

        case S:
            if (i == graph->size) { // if the vertex is on the bottom row
                return;
            }
            vertex->edges[S].vertex = &graph->arr[i + 1][j];
            vertex->edges[S].weight = 1;
            break;

        case NW:
            if (j == 0 || i == 0) { // if the vertex is on a top or left edge
                return;
            }
            vertex->edges[NW].vertex = &graph->arr[i - 1][j - 1];
            vertex->edges[NW].weight = 1.414;
            break;

        case NE:
            if (j == graph->size || i == 0) { // if the vertex is on a top or right edge
                return;
            }
            vertex->edges[NE].vertex = &graph->arr[i - 1][j + 1];
            vertex->edges[NE].weight = 1.414;
            break;

        case SW:
            if (j == 0 || i == graph->size) { // if the vertex is on a bottom or left edge
                return;
            }
            vertex->edges[SW].vertex = &graph->arr[i + 1][j - 1];
            vertex->edges[SW].weight = 1.414;
            break;

        case SE:
            if (j == graph->size || i == graph->size) { // if the vertex is on a bottom or right edge
                return;
            }
            vertex->edges[SE].vertex = &graph->arr[i + 1][j + 1];
            vertex->edges[SE].weight = 1.414;
            break;

        default:
            exit(1);
    }
}

void graph_init(graph_s_t *graph) {
    if (!graph) return;
    for (int i = 0; i < graph->size; ++i) {
        for (int j = 0; j < graph->size; ++j) {
            vertex_s_t *vertex = &graph->arr[i][j];
            vertex->coord.x = j;
            vertex->coord.y = graph->size - i - 1;
            vertex->g = DBL_MAX;
            vertex->parent = NULL;
            for (int k = 0; k < 8; ++k) {
                set_weights(graph, i, j, k);
            }
        }
    }
}

void graph_reset(graph_s_t *graph) {
    if (!graph) return;
    for (int i = 0; i < graph->size; ++i) {
        for (int j = 0; j < graph->size; ++j) {
            graph->arr[i][j].deleted = 0;
        }
    }
}

void graph_set_size(graph_s_t *graph, int size) {
    if (!graph) return;
    for (int i = 0; i < graph->size; ++i) {
        free(graph->arr[i]);
    }
    free(graph->arr);
    graph->size = size;
    // HACK: some of the edges were being assigned invalid addresses, adding a buffer seems to fix this
    ++size;
    graph->arr = malloc(sizeof(vertex_s_t *) * size);
    for (int i = 0; i < size; ++i) {
        graph->arr[i] = malloc(sizeof(vertex_s_t) * size);
        memset(graph->arr[i], 0, sizeof(vertex_s_t) * size);
    }
    graph_init(graph);
}

graph_s_t *graph_new(int size) {
    graph_s_t *ret = malloc(sizeof(graph_s_t));
    ret->size = size;
    // HACK: some of the edges were being assigned invalid addresses, adding a buffer seems to fix this
    ++size;
    ret->arr = malloc(sizeof(vertex_s_t *) * size);
    for (int i = 0; i < size; ++i) {
        ret->arr[i] = malloc(sizeof(vertex_s_t) * size);
        memset(ret->arr[i], 0, sizeof(vertex_s_t) * size);
    }
    graph_init(ret);
    return ret;
}

void graph_free(graph_s_t *graph) {
    if (!graph) return;
    for (int i = 0; i < graph->size; ++i) {
        free(graph->arr[i]);
    }
    free(graph->arr);
    free(graph);
}
