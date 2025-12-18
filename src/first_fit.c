#include "allocators.h"
#include <stdlib.h>
#include <string.h>

typedef struct BlockHeader {
    size_t size;
    int free;
    struct BlockHeader* next;
} BlockHeader;

typedef struct {
    Allocator base;
    BlockHeader* head;
} FirstFitAlloc;

static void* ff_alloc(Allocator* a, size_t size) {
    FirstFitAlloc* alloc = (FirstFitAlloc*) a;
    BlockHeader* cur = alloc->head;

    while (cur) {
        if (cur->free && cur->size >= size) {
            size_t remaining = cur->size - size;

            if (remaining > sizeof(BlockHeader) + 8) {
                BlockHeader* new_block = (BlockHeader*)((char*)cur + sizeof(BlockHeader) + size);
                new_block->size = remaining - sizeof(BlockHeader);
                new_block->free = 1;
                new_block->next = cur->next;

                cur->size = size;
                cur->next = new_block;
            }

            cur->free = 0;
            return (char*)cur + sizeof(BlockHeader);
        }
        cur = cur->next;
    }
    return NULL;
}

static void ff_free(Allocator* a, void* ptr) {
    if (!ptr) return;

    BlockHeader* block = (BlockHeader*)((char*)ptr - sizeof(BlockHeader));
    block->free = 1;

    BlockHeader* next = block->next;

    // merge with next
    if (next && next->free) {
        block->size += sizeof(BlockHeader) + next->size;
        block->next = next->next;
    }
}

Allocator* create_first_fit(void* memory, size_t size) {
    FirstFitAlloc* alloc = (FirstFitAlloc*)malloc(sizeof(FirstFitAlloc));

    alloc->base.memory = memory;
    alloc->base.size = size;
    alloc->base.alloc = ff_alloc;
    alloc->base.free = ff_free;

    alloc->head = (BlockHeader*)memory;
    alloc->head->size = size - sizeof(BlockHeader);
    alloc->head->free = 1;
    alloc->head->next = NULL;

    return (Allocator*)alloc;
}
