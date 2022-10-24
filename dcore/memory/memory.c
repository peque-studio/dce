#include <dcore/memory.h>
#include <dcore/common.h>
#include <stdlib.h>

DCmemAllocStats allocStats;

void *dcmemAllocate(size_t size) {
    if(size == 0) DCD_WARNING("Tried to allocate zero bytes.");
    allocStats.allocCount += 1;
    return malloc(size);
}

void dcmemDeallocate(void *pointer) {
    if(pointer == NULL) DCD_WARNING("Tried to deallocate a NULL pointer.");
    allocStats.deallocCount += 1;
    free(pointer);
}

void *dcmemReallocate(void *pointer, size_t size) {
    if(pointer == NULL) DCD_WARNING("Tried to reallocate a NULL pointer.");
    if(size == 0) DCD_WARNING("Tried to reallocate to zero bytes.");
    allocStats.reallocCount += 1;
    return realloc(pointer, size);
}
