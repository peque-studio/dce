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

void *dcmemAllocate_(size_t size, const char *f, const char *u, int l);
void dcmemDeallocate_(void *pointer, const char *f, const char *u, int l);
void *dcmemReallocate_(void *pointer, size_t size, const char *f, const char *u, int l);
#define dcmemAllocate(size) dcmemAllocate_((size), __FILE__, __func__, __LINE__)
#define dcmemDeallocate(pointer) dcmemDeallocate_((pointer), __FILE__, __func__, __LINE__)
#define dcmemReallocate(pointer, size) dcmemReallocate_((pointer), (size), __FILE__, __func__, __LINE__)

#else
#  include <stdlib.h>
#  define dcmemAllocate(size) malloc(size)
#  define dcmemDeallocate(pointer) free(pointer)
#  define dcmemReallocate(pointer, size) reallocate(pointer, size)
#endif

#define DCMEM_PUSH(ARENA, TYPE) (((TYPE)*)dcmemPush((ARENA), sizeof(TYPE)))
#define DCMEM_POP(ARENA, TYPE) dcmemPop((ARENA), sizeof(TYPE))

void printAllocationStats();

#endif
