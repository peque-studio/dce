#include <dcore/memory.h>
#include <dcore/common.h>

extern DCmemAllocStats allocStats; // defined in dcore/memory/memory.c

void *dcmemPush(DCmemArena *arena, size_t size) {
    if(arena->size == 0) {
        arena->top = size;
        arena->memory = dcmemAllocate(arena->size = arena->step);
    } else {
        arena->top += size;
        arena->memory = dcmemReallocate(arena->memory, arena->size += arena->step);
    }

    return (uint8_t*)arena->memory + arena->top;
}

void dcmemPop(DCmemArena *arena, size_t size) {
    if(arena->top < size) {
        DCD_FATAL("Tried to pop %zu bytes from an arena with only %zu bytes left.", size, arena->top);
        arena->top = 0;
        return;
    }

    arena->top -= size;
    if(arena->size - arena->top >= arena->step * 2) {
        arena->memory = dcmemReallocate(arena->memory, arena->size -= arena->step);
    }
}
