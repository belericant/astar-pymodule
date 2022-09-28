#ifndef ASTAR_GRAPH_H
#define ASTAR_GRAPH_H

// forward declaration
typedef struct edge_s edge_s_t;
typedef struct vertex_s vertex_s_t;

// struct definition
typedef struct point_s {
    int x, y;
} point_s_t;

typedef struct edge_s {
    vertex_s_t *vertex;
    double weight;
} edge_s_t;

typedef struct vertex_s {
    edge_s_t edges[8];
    int deleted;
    point_s_t coord;
    double f, g;
    vertex_s_t *parent;
} vertex_s_t;

typedef enum adjacent_vertex_e {
    NW = 0, N, NE, W, E, SW, S, SE,
} adjacent_vertex_e_t;

typedef struct graph_s {
    vertex_s_t **arr;
    int size;
} graph_s_t;

void graph_init(graph_s_t *);

void graph_reset(graph_s_t *);

void graph_set_size(graph_s_t *, int);

graph_s_t *graph_new(int);

void graph_free(graph_s_t *);

#endif //ASTAR_GRAPH_H
