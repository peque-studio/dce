#ifndef DCORE_MEMORY_H
#define DCORE_MEMORY_H
#include <dcore/common.h>

typedef struct DCmemArena {
    size_t size;
    size_t top;
    size_t step;
    void *memory;
} DCmemArena;

void *dcmemPush(DCmemArena *arena, size_t size);
void dcmemPop(DCmemArena *arena, size_t size);

void *dcmemAllocate(size_t size);
void dcmemDeallocate(void *pointer);
void *dcmemReallocate(void *pointer, size_t size);

#define DCMEM_PUSH(ARENA, TYPE) (((TYPE)*)dcmemPush((ARENA), sizeof(TYPE)))
#define DCMEM_POP(ARENA, TYPE) dcmemPop((ARENA), sizeof(TYPE))

#endif
