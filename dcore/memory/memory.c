#include <dcore/memory.h>
#include <dcore/common.h>
#include <stdlib.h>

DCmemAllocStats allocStats;

#if defined(DC_DEBUG)

void *dcmemAllocate_(size_t size, const char *f, const char *u, int l) {
	if(size == 0) DCD_WARNING("Tried to allocate zero bytes. %s:%d @%s", f, l, u);
	allocStats.allocCount += 1;
	return malloc(size);
}

void dcmemDeallocate_(void *pointer, const char *f, const char *u, int l) {
	if(pointer == NULL) DCD_WARNING("Tried to deallocate a NULL pointer. %s:%d @%s", f, l, u);
	allocStats.deallocCount += 1;
	free(pointer);
}

void *dcmemReallocate_(void *pointer, size_t size, const char *f, const char *u, int l) {
	if(pointer == NULL) DCD_WARNING("Tried to reallocate a NULL pointer. %s:%d @%s", f, l, u);
	if(size == 0) DCD_WARNING("Tried to reallocate to zero bytes. %s:%d @%s", f, l, u);
	allocStats.reallocCount += 1;
	return realloc(pointer, size);
}

#endif
