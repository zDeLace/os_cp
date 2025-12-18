#include "allocators.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MEM_SIZE (1<<20) // 1 MB
#define OPS 50000

static double time_alloc(Allocator* a) {
    void* ptrs[OPS];
    clock_t t = clock();

    for (int i = 0; i < OPS; i++)
        ptrs[i] = a->alloc(a, 32 + (rand() % 256));

    for (int i = 0; i < OPS; i++)
        a->free(a, ptrs[i]);

    return (double)(clock() - t) / CLOCKS_PER_SEC;
}

int main() {
    void* mem1 = malloc(MEM_SIZE);
    void* mem2 = malloc(MEM_SIZE);

    Allocator* ff = create_first_fit(mem1, MEM_SIZE);
    Allocator* buddy = create_buddy(mem2, MEM_SIZE);

    srand(time(NULL));

    double t1 = time_alloc(ff);
    double t2 = time_alloc(buddy);

    printf("First fit time: %.5f sec\n", t1);
    printf("Buddy time:     %.5f sec\n", t2);

    free(mem1);
    free(mem2);
    return 0;
}
