#ifndef ASTAR_ASTAR_H
#define ASTAR_ASTAR_H

int astar(graph_s_t *graph, vertex_s_t *start, vertex_s_t *end, double (*h)(int, int, int, int));

void add_obstacle(graph_s_t *graph, int x, int y);

void remove_obstacle(graph_s_t *graph, int x, int y);

void modify_weights(graph_s_t *graph, int x1, int y1, int x2, int y2, double weight);

point_s_t coord_to_index(graph_s_t *graph, int x, int y);

double heuristic(int x, int y, int goal_x, int goal_y);

#endif //ASTAR_ASTAR_H
