/* RandomArt
 * Memory Pool implementation
 */

#include <stdio.h>
#include <stdlib.h>

#include "mempool.h"

static MemRegion *_newRegion(void) {
	MemRegion *region = malloc(POOL_SIZE);
	if( region == NULL ) {
		fprintf(stderr, "not enough memory");
		exit(EXIT_FAILURE);
	}

	region->next = NULL;
	region->data = (byte *)&region[1];
	region->end = region->data + POOL_SIZE;

	return region;
}

MemPool poolNew(void) {
	MemPool pool;
	pool.start = _newRegion();
	pool.end = pool.start;

	return pool;
}

void *poolAlloc(MemPool *pool, int size) {
	MemRegion *region = pool->end;
	if( region->data + size >= region->end - 1 ) {
		region->next = _newRegion();
		region = region->next;
		pool->end = region;
	}

	void *mem = (void *)region->data;
	region->data += size;
	return mem;
}

void poolFree(MemPool *pool) {
	MemRegion *region = pool->start;
	while( region != NULL ) {
		MemRegion *next = region->next;
		free(region);
		region = next;
	}

	pool->start = NULL;
	pool = NULL;
}
