#ifndef DCORE_MEMORY_H
#define DCORE_MEMORY_H
#include <stddef.h>

typedef struct DCmemArena {
    size_t size;
    size_t top;
    size_t step;
    void *memory;
} DCmemArena;

typedef struct DCmemAllocStats {
    size_t allocCount, deallocCount, reallocCount;
    size_t arenaPushCount, arenaPopCount;
} DCmemAllocStats;

void *dcmemPush(DCmemArena *arena, size_t size);
void dcmemPop(DCmemArena *arena, size_t size);

#if defined(DC_DEBUG)

void *dcmemAllocate(size_t size);
void dcmemDeallocate(void *pointer);
void *dcmemReallocate(void *pointer, size_t size);

#endif

#define DCMEM_PUSH(ARENA, TYPE) (((TYPE)*)dcmemPush((ARENA), sizeof(TYPE)))
#define DCMEM_POP(ARENA, TYPE) dcmemPop((ARENA), sizeof(TYPE))

void printAllocationStats();

#endif
