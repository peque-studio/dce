#include <dcore/memory.h>
#include <stdlib.h>

static size_t allocationCount = 0;
static size_t deallocationCount = 0;
static size_t reallocationCount = 0;

void *dcmemAllocate(size_t size) {
    allocationCount += 1;
    return malloc(size);
}

void dcmemDeallocate(void *pointer) {
    deallocationCount += 1;
    free(pointer);
}

void *dcmemReallocate(void *pointer, size_t size) {
    reallocationCount += 1;
    return realloc(pointer, size);
}
