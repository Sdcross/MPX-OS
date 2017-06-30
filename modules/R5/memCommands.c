//
// Created by nas on 4/6/17.
//

#include <mem/memoryControl.h>
#include <boolean.h>
#include <core/comHandler.h>
#include <core/help.h>
#include <modules/mpx_supt.h>
#include <modules/R5/memCommands.h>
#include <modules/R5/help.h>
#include <system.h>

/**
 * Registers the permanent commands in the command handler
 */
void registerR5TempCommands() {
    addFunctionDef("initHeap", HELP_R5_COMMAND_HEAP, initHeap);
    addFunctionDef("allocMem", HELP_R5_COMMAND_ALLOC, allocateMem);
    addFunctionDef("freeMem", HELP_R5_COMMAND_FREE, freeMemory);
	addFunctionDef("isEmpty", HELP_R5_COMMAND_EMPTY, isEmptyCom);
}

/**
 * Initializes the heap to the provided size and creates a free mem block across it
 *
 * @return True or false
 */
const char *initHeap(char **args, int numArgs){
    if (numArgs != 1) {
		return HELP_R5_COMMAND_HEAP;
	}
    if (initializeHeap(atoi(args[0]))) {
		return "True";
	}
    else {
		return "False";
	}
}

/**
 * Allocates a memory block if enough memory is available
 *
 * @param size - size of memory to allocate in bytes
 * @return pointer to the me
 */
const char *allocateMem(char **args, int numArgs){
    if (numArgs != 1) {
		return HELP_INVALID_ARGUMENTS;
	}
    int memAddress = (int)allocateMemory(atoi(args[0]));
    char* endString = sys_alloc_mem(50);
    itoa(memAddress, endString, 10);
    return endString;
}

/**
 * Deallocates the block of memory at the mempointer
 *
 * @param memPointer - pointer to the mem block
 */
const char *freeMemory(char **args, int numArgs){
    if (numArgs != 1) {
		return HELP_INVALID_ARGUMENTS;
	}
    boolean ret = deallocateMemory((void*)(atoi(args[0])));
	if (ret == true) {
		return "Memory Freed";
	}
	else {
		return "Failed to free memory";
	}
}

/**
 * Check if memory is empty
 */
const char *isEmptyCom(char **args, int numArgs){
	no_warn(args);
	no_warn(numArgs);
	if (isEmpty()) {
		return "The heap is empty";
	}
	else {
		return "The heap is not empty";
	}
}