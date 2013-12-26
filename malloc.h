/*
 * malloc.h
 *
 *  Created on: Dec 23, 2013
 *      Author: xkx
 */

#ifndef MALLOC_H_
#define MALLOC_H_

#define HEAP_SIZE 150

typedef struct HeapMeta {
	char isUsed;
	usize_t size;
} HeapMeta;

static char heap[HEAP_SIZE];
static volatile usize_t heap_index = 0;

void* malloc(usize_t size);

void free(void* ptr);

#endif /* MALLOC_H_ */
