#include "allocators.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define MAX_ORDER 24

typedef struct Buddy {
    Allocator base;
    void* free_lists[MAX_ORDER + 1];
    int max_order;
} Buddy;

static int get_order(size_t size) {
    int order = 0;
    size_t n = 1;
    while (n < size) {
        n <<= 1;
        order++;
    }
    return order;
}

static void buddy_add(Buddy* b, void* block, int order) {
    *(void**)block = b->free_lists[order];
    b->free_lists[order] = block;
}

static void* buddy_remove(Buddy* b, int order) {
    void* block = b->free_lists[order];
    if (block)
        b->free_lists[order] = *(void**)block;
    return block;
}

static void* buddy_alloc_fn(Allocator* a, size_t size) {
    Buddy* b = (Buddy*)a;

    size_t total = size + sizeof(int);
    int order = get_order(total);

    int current = order;
    while (current <= b->max_order && !b->free_lists[current])
        current++;

    if (current > b->max_order) return NULL;

    void* block = buddy_remove(b, current);

    while (current > order) {
        current--;
        void* buddy = (char*)block + (1 << current);
        buddy_add(b, buddy, current);
    }

    *(int*)block = order;
    return (char*)block + sizeof(int);
}

static void buddy_free_fn(Allocator* a, void* ptr) {
    if (!ptr) return;

    Buddy* b = (Buddy*)a;

    char* block = (char*)ptr - sizeof(int);
    int order = *(int*)block;

    while (1) {
        size_t size = 1 << order;
        size_t offset = block - (char*)b->base.memory;

        size_t buddy_offset = offset ^ size;
        char* buddy = (char*)b->base.memory + buddy_offset;

        void** cur = &b->free_lists[order];
        int merged = 0;

        while (*cur) {
            if (*cur == buddy) {
                *cur = *(void**)*cur;
                if (buddy < block)
                    block = buddy;
                merged = 1;
                break;
            }
            cur = (void**)*cur;
        }

        if (!merged) break;
        order++;
    }

    buddy_add(b, block, order);
}

Allocator* create_buddy(void* memory, size_t size) {
    Buddy* b = (Buddy*)malloc(sizeof(Buddy));

    b->base.memory = memory;
    b->base.size = size;
    b->base.alloc = buddy_alloc_fn;
    b->base.free = buddy_free_fn;

    memset(b->free_lists, 0, sizeof(b->free_lists));

    int order = get_order(size);
    b->max_order = order;

    buddy_add(b, memory, order);

    return (Allocator*)b;
}
