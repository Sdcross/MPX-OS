//
// Created by djbowman on 4/8/17.
//
#include <system.h>
#include <mem/heap.h>
#include <mem/memoryControl.h>
#include <modules/mpx_supt.h>
#include <boolean.h>

cmcb *freeHead;
cmcb *allocatedHead;
void *memHeap;
int isInitialized = false;
int memSize;
int memAllocated;

cmcb *_placeStructs(int size, void *pos, int type, cmcb *prev, cmcb *next);

/**
 * Private helper function to create structs to denote the beginning and end of a memory block
 *
 * @param size - size of block in bytes
 * @param pos - mem location of beginning
 * @param type - type of mem block, either ALLOCATED or FREE
 * @param prev - pointer to previous cmcb
 * @param next - pointer to next cmcb
 * @return pointer to create cmcb at beginning of memory block
 */
cmcb *_placeStructs(int size, void *pos, int type, cmcb *prev, cmcb *next){
	cmcb *firstCMCB = (struct cmcb*) pos;
	firstCMCB->type = type;
	firstCMCB->beginningAddr = pos + sizeof(struct cmcb);
	firstCMCB->size = size;
	firstCMCB->memSize = size - sizeof(struct cmcb) - sizeof(struct lmcb);
	firstCMCB->name = "FREE";
	firstCMCB->next = next;
	firstCMCB->prev = prev;

	lmcb *firstLMCB = (struct lmcb*)(size - sizeof(struct lmcb));
	firstLMCB->type = type;
	firstLMCB->size = size;
	firstLMCB->memSize = size - sizeof(struct cmcb) - sizeof(struct lmcb);

	return firstCMCB;
}

/**
 * Private helper function to merge all adjacent memory blocks
 *
 */
void _mergeAdjacentFree(){
	boolean isDoneMerging = false;
	cmcb* node = freeHead;

	boolean madeChanges = false;

	while (!isDoneMerging){
		if (node == NULL){ //end of list
			if (madeChanges){ //have made changes, need to reloop list
				madeChanges = false;
				node = freeHead;
			}
			else { //no changes made on this pass, done
				isDoneMerging = true;
			}
		}
		else if ((void*)node + node->size == (void*)node->next){ //need merged
			cmcb *next = node->next->next; //store pertinent info
			cmcb *prev = node->prev;
			int trueSize = node->size + node->next->size;
			 cmcb *newNode = _placeStructs(trueSize, (void*)node, FREE, prev, next); //make new free mem node
			if (prev != NULL){
				prev->next = newNode;
			}
			if (next != NULL){
				next->prev = newNode;
			}

			madeChanges = true;
		}
		else {
			node = node->next; //iterate
		}
	}
}

/**
 * Initializes the heap to the provided size and creates a free mem block across it
 *
 * @param size - size of heap in bytes
 * @return boolean - boolean denoting if heap was initialized
 */
boolean initializeHeap(int size){
	if (size <= 0){
		return false;
	}
	if (!isInitialized){ //dont try to reinit
		isInitialized = true;
		memAllocated = 0;
		memSize = size;

		//allocate memHeap
		memHeap = (void*)kmalloc(sizeof(struct cmcb) + sizeof(struct lmcb) + size);
		//create bounding structs for all of free memory
		freeHead = _placeStructs(size + sizeof(struct cmcb) + sizeof(struct lmcb), memHeap, FREE, NULL, NULL);
		//initialize allocated head
		allocatedHead = NULL;

		return true;
	}
	return false;
}

/**
 * Allocates a memory block if enough memory is availabel
 *
 * @param size - size of memory to allocate in bytes
 * @return pointer to the me
 */
void *allocateMemory(int size){
	if (!isInitialized){ //not init
		return NULL;
	}
	if (size == 0){ //invalid params
		return NULL;
	}
	if (memSize - memAllocated < (int)(sizeof(struct cmcb) + sizeof(struct lmcb) + size)){ //not enough mem
		return NULL;
	}

	int trueSize = sizeof(struct cmcb) + sizeof(struct lmcb) + size; //true size that new node will need, size + struct sizes
	boolean memPosNotFound = true;
	cmcb *freeList = freeHead;
	while (memPosNotFound){
		if (freeList == NULL){ //end of memory, not a large enough contiguous chunk of memory
			return NULL;
		}
		if (freeList->memSize >= trueSize){ //found spot
			memPosNotFound = false;
		}
		else { //go to next node
			freeList = freeList->next;
		}
	}
	//store needed information from freeList node
	cmcb *prev = freeList->prev;
	cmcb *next = freeList->next;
	void *addr = (void *)freeList;
	int prevSize = freeList->size;

	cmcb *newAlloc = _placeStructs(trueSize, addr, ALLOCATED, NULL, NULL); //make new allocated
	newAlloc->name = getCOPName();
	memAllocated += trueSize;
	cmcb *newFree = _placeStructs(prevSize - trueSize, (void*)newAlloc + newAlloc->size, FREE, prev, next); //make new free
	if (prev != NULL){
		prev->next = newFree;
	}
	if (next != NULL){
		next->prev = newFree;
	}
	if(newFree->prev == NULL){
		freeHead = newFree;
	}

	boolean spotFound = false;
	if (allocatedHead == NULL) {
		allocatedHead = newAlloc;
		return newAlloc->beginningAddr;
	}
	cmcb *node = allocatedHead;
	while(!spotFound){
		if (newAlloc < node){
			newAlloc->next = node;
			newAlloc->prev = node->prev;
			newAlloc->prev->next = newAlloc;
			node->prev = newAlloc;
			if (allocatedHead == node) {
				allocatedHead = newAlloc;
			}
			spotFound = true;
		}
		else if (node->next == NULL){
			node->next = newAlloc;
			newAlloc->prev = node;
			spotFound = true;
		}
		else {
			node = node->next;
		}
	}
	return newAlloc->beginningAddr;
}

/**
 * Deallocates the block of memory at the mempointer
 *
 * @param memPointer - pointer to the mem block
 * @return boolean - boolean telling whether succesful dealloc
 */
boolean deallocateMemory(void *memPointer){
	boolean isFound = false;
	cmcb *node = allocatedHead;

	while (!isFound){
		if (node == NULL){ //reach end, not found
			return false;
		}
		if (node->beginningAddr == memPointer){
			isFound = true;
		}
		else{
			node = node->next;
		}
	}
	if (node == allocatedHead){
		allocatedHead = allocatedHead->next;

		if (allocatedHead != NULL) {
			allocatedHead->prev = NULL;
		}
	}
	else {
		if (node->next != NULL){
			node->next->prev = node->prev;
		}
		if (node->prev != NULL){
			node->prev->next = node->next;
		}
	}

	cmcb *newFree = _placeStructs(node->size, (void*)node, FREE, NULL, NULL); //make new free block
	memAllocated -= newFree->size;
	if (freeHead == NULL){
		freeHead = newFree;
	}
	else {
		boolean spotFound = false;
		cmcb *newNode = freeHead;
		while (!spotFound){
			if (newFree < newNode){
				newFree->prev = newNode->prev;
				newFree->next = newNode;
				newNode->prev->next = newFree;
				newNode->prev = newFree;
				spotFound = true;

				if (newNode == freeHead){
					freeHead = newFree;
				}
			}
			else if (newNode->next == NULL){
				newNode->next = newFree;
				newFree->prev = newNode;
				spotFound = true;
			}
			else {
				newNode = newNode->next;
			}
		}
	}
	_mergeAdjacentFree();
	return true;
}

/**
 * Returns a boolean telling if all the memory is empty
 *
 * @return boolean
 */
boolean isEmpty(){
	if (memAllocated == 0){
		return true;
	}
	return false;
}

/**
 * Returns the head of the free list
 *
 * @return cmcb * to the free list head
 */
cmcb *getFreeHead() {
	return freeHead;
}

/**
 * Returns the head to the allocated list
 *
 * @return cmcb * to the allocated list head
 */
cmcb *getAllocatedHead() {
	return allocatedHead;
}