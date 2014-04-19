#include "std.h"
#include "malloc.h"

// static memory allocator, inspired by FreeRTOS heap_2.c
// supports allocating/freeing memory, but does not merge free memory fragments

/**
 * Static memory allocator
 *
 * @param size  size (in bytes) of memory needed
 *              must be > 0 and < `HEAP_SIZE`
 *
 * @return address of allocated memory,
 *         or `NULL` if out of memory
 *
 */

void* malloc(usize_t size) {
	HeapMeta* meta;
	if (size + sizeof(HeapMeta) <= HEAP_SIZE - heap_index) {
		meta = (HeapMeta*) (heap + heap_index);
		meta->isUsed = 1;
		meta->size = size;
		heap_index += size;
		return meta + sizeof(HeapMeta);
	}
	else {
		for (meta = (HeapMeta*) heap;
			 meta < (HeapMeta*) (heap + HEAP_SIZE);
			 meta += meta->size + sizeof(HeapMeta)) {
			if(!meta->isUsed) {
				meta->isUsed = 1;
				return meta + sizeof(HeapMeta);
			}
		}
	}
	return NULL;
}

/**
 * Release allocated memory
 *
 * @param ptr   valid pointer to an allocated memory chunk
 *
 * The behavior is undefined if `ptr` points to memory that
 * is not on heap or has been freed before
 * CAREFUL: may cause gigantic explosion
 *
 */

void free(void* ptr) {
	HeapMeta* meta = (HeapMeta*)ptr - sizeof(HeapMeta);
	meta->isUsed = 0;
	if ((char*) ptr + meta->size == heap + heap_index) {
		heap_index -= meta->size + sizeof(HeapMeta);
	}
}
