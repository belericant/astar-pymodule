#include "heap.h"
#include <string.h>
#include <stdlib.h>

typedef struct elem_s {
    unsigned int idx;
    void *ptr;
    heap_s_t *heap;
} elem_s_t;

inline static void swap(heap_s_t *heap, unsigned int i, unsigned int j) {
    void *temp = heap->arr[i];
    heap->arr[i] = heap->arr[j];
    heap->arr[j] = temp;
    ((elem_s_t *) hashmap_get(heap->map, &(elem_s_t) {.heap = heap, .ptr = heap->arr[i]}))->idx = i;
    ((elem_s_t *) hashmap_get(heap->map, &(elem_s_t) {.heap = heap, .ptr = heap->arr[j]}))->idx = j;
}

uint64_t mem_hash(const void *item, uint64_t seed0, uint64_t seed1) {
    elem_s_t *elem = (elem_s_t *) item;
    uintptr_t addr = (uintptr_t) elem->ptr;
    return addr;
}

int map_compare(const void *ptr1, const void *ptr2, void *udata) {
    elem_s_t *elem1 = (elem_s_t *) ptr1;
    elem_s_t *elem2 = (elem_s_t *) ptr2;
    return elem1->heap->comparator(elem1->ptr, elem2->ptr);
}

heap_s_t *heap_new(unsigned int initialLen, int (*comparator)(const void *, const void *)) {
    heap_s_t *heap = (heap_s_t *) malloc(sizeof(heap_s_t));
    heap->arr = malloc(sizeof(void *) * initialLen);
    memset(heap->arr, 0, sizeof(void *) * initialLen);
    heap->idx = 0;
    heap->len = initialLen;
    heap->comparator = comparator;
    heap->map = hashmap_new(sizeof(elem_s_t), initialLen, 0, 0, mem_hash, map_compare, NULL);
    return heap;
}

void heap_clear(heap_s_t *heap) {
    if (!heap) return;
    heap->idx = 0;
    hashmap_clear(heap->map, 1);
    memset(&heap->arr, 0, sizeof(void *) * heap->len);
}

void heap_push_back(heap_s_t *heap, void *ptr) {
    if (!heap || !ptr) return;
    if (++heap->idx >= heap->len) {
        heap->arr = realloc(heap->arr, heap->len * 2);
        heap->len = heap->len * 2;
    }
    heap->arr[heap->idx] = ptr;
    hashmap_set(heap->map, &(elem_s_t) {.heap = heap, .ptr = ptr, .idx = heap->idx});
    unsigned int i = heap->idx;
    while (i > 1) {
        if (heap->comparator(heap->arr[i], heap->arr[i / 2]) < 0) {
            swap(heap, i, i / 2);
            i = i / 2;
        } else {
            break;
        }
    }
}

void *heap_pop_min(heap_s_t *heap) {
    if (!heap) return NULL;
    void *ret = heap->arr[1];
    hashmap_delete(heap->map, &(elem_s_t) {.heap = heap, .ptr = ret, .idx = 1});
    heap->arr[1] = heap->arr[heap->idx];
    heap->arr[heap->idx--] = NULL;
    if (heap->arr[1]) {
        ((elem_s_t *) hashmap_get(heap->map, &(elem_s_t) {.heap = heap, .ptr = heap->arr[1]}))->idx = 1;
    }
    unsigned int i = 1;
    while (2 * i < heap->idx) {
        if (2 * i + 1 > heap->idx || heap->comparator(heap->arr[2 * i], heap->arr[2 * i + 1]) < 0) {
            if (heap->comparator(heap->arr[i], heap->arr[2 * i]) > 0) {
                swap(heap, i, 2 * i);
                i = 2 * i;
            } else {
                break;
            }
        } else {
            if (heap->comparator(heap->arr[i], heap->arr[2 * i + 1]) > 0) {
                swap(heap, i, 2 * i + 1);
                i = 2 * i + 1;
            } else {
                break;
            }
        }
    }
    return ret;
}

void *heap_peek_min(heap_s_t *heap) {
    if (!heap) NULL;
    return heap->arr[1];
}

int heap_contains(heap_s_t *heap, void *ptr) {
    if (!heap || !ptr) return 0;
    return hashmap_get(heap->map, &(elem_s_t) {.heap = heap, .ptr = ptr}) != NULL;
}

void heap_decrease_key(heap_s_t *heap, void *ptr) {
    if (!heap || !ptr) return;
    elem_s_t *elem = (elem_s_t *) hashmap_get(heap->map, &(elem_s_t) {.heap = heap, .ptr = ptr});
    if (elem) {
        unsigned int i = elem->idx;
        while (i > 1) {
            if (heap->comparator(heap->arr[i], heap->arr[i / 2]) < 0) {
                swap(heap, i, i / 2);
                i = i / 2;
            } else {
                break;
            }
        }
    }
}

void heap_free(heap_s_t *heap) {
    if (!heap) return;
    free(heap->arr);
    hashmap_free(heap->map);
    free(heap);
}
