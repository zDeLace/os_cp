#ifndef ALLOCATORS_H
#define ALLOCATORS_H

#include <stddef.h>

typedef struct Allocator Allocator;

typedef void* (*alloc_fn)(Allocator*, size_t);
typedef void (*free_fn)(Allocator*, void*);

struct Allocator {
    void* memory;
    size_t size;

    alloc_fn alloc;
    free_fn free;
};

// FIRST-FIT
Allocator* create_first_fit(void* memory, size_t size);

// BUDDY
Allocator* create_buddy(void* memory, size_t size);

#endif
