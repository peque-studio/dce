#include <dcore/memory.h>
#include <stdlib.h>

DCmemAllocStats allocStats;

void *dcmemAllocate(size_t size) {
    allocStats.allocCount += 1;
    return malloc(size);
}

void dcmemDeallocate(void *pointer) {
    allocStats.deallocCount += 1;
    free(pointer);
}

void *dcmemReallocate(void *pointer, size_t size) {
    allocStats.reallocCount += 1;
    return realloc(pointer, size);
}
