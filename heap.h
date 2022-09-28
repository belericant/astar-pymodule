#ifndef ASTAR_HEAP_H
#define ASTAR_HEAP_H

#include "graph.h"
#include "hashmap.h"
#include <stddef.h>

typedef struct heap_s {
    void **arr;
    unsigned int idx, len;

    // neg if first < second, 0 if equal, pos if first > second
    int (*comparator)(const void *, const void *);

    hashmap_s_t *map;
} heap_s_t;

heap_s_t *heap_new(unsigned int, int (const void *, const void *));

void heap_clear(heap_s_t *);

void *heap_pop_min(heap_s_t *);

void *heap_peek_min(heap_s_t *);

void heap_push_back(heap_s_t *, void *);

void heap_decrease_key(heap_s_t *, void *);

int heap_contains(heap_s_t *, void *);

void heap_free(heap_s_t *);

#endif //ASTAR_HEAP_H
