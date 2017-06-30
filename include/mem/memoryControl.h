#ifndef _MEM_MEMCONTROL
#define _MEM_MEMCONTROL

//
// Created by djbowman on 4/8/17.
//
#include <system.h>
#include <mem/heap.h>
#include <modules/mpx_supt.h>
#include <boolean.h>

#define FREE 0
#define ALLOCATED 1

typedef struct cmcb{
	int type;
	void *beginningAddr;
	int size;
	int memSize;
	const char *name;

	struct cmcb *next;
	struct cmcb *prev;
} cmcb;

typedef struct lmcb{
	int type;
	int size;
	int memSize;
} lmcb;

/**
 * Initializes the heap to the provided size and creates a free mem block across it
 *
 * @param size - size of heap in bytes
 * @return boolean - boolean denoting if heap was initialized
 */
boolean initializeHeap(int size);

/**
 * Allocates a memory block if enough memory is availabel
 *
 * @param size - size of memory to allocate in bytes
 * @return pointer to the me
 */
void *allocateMemory(int size);

/**
 * Deallocates the block of memory at the mempointer
 *
 * @param memPointer - pointer to the mem block
 * @return boolean - tells whether successfull dealloc
 */
boolean deallocateMemory(void *memPointer);

/**
 * Returns a boolean telling if all the memory is empty
 *
 * @return boolean
 */
boolean isEmpty();

/**
 * Returns the head of the free list
 *
 * @return cmcb * to the free list head
 */
cmcb *getFreeHead();

/**
 * Returns the head to the allocated list
 *
 * @return cmcb * to the allocated list head
 */
cmcb *getAllocatedHead();

#endif