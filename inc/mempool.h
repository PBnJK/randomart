#ifndef GUARD_RANDOMART_MEMPOOL_H_
#define GUARD_RANDOMART_MEMPOOL_H_

#include <stdint.h>

typedef uint8_t byte;

#define POOL_SIZE 8192

typedef struct _MemRegion {
	struct _MemRegion *next;
	byte *data, *end;
} MemRegion;

typedef struct _MemPool {
	MemRegion *start, *end;
} MemPool;

/* Creates a new memory pool */
MemPool poolNew(void);

/* Allocates size bytes in the given memory pool */
void *poolAlloc(MemPool *pool, int size);

/* Frees the memory pool */
void poolFree(MemPool *pool);

#endif // !GUARD_RANDOMART_MEMPOOL_H_
