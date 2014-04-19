/*
 * malloc.h
 *
 *  Created on: Dec 23, 2013
 *      Author: xkx
 */

#ifndef MALLOC_H_
#define MALLOC_H_

/// Size of the static memory heap
#define HEAP_SIZE 150

/// Internal structure of a heap block
typedef struct HeapMeta {
	char isUsed;
	usize_t size;
} HeapMeta;

// For internal use only
// heap and heap cursor declaration
static char heap[HEAP_SIZE];
static volatile usize_t heap_index = 0;

// Memory allocation
void* malloc(usize_t size);

// Memory release
void free(void* ptr);

#endif /* MALLOC_H_ */
