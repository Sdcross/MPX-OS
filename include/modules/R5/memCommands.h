#ifndef _MODULES_R5_COMMANDS //include these two but for R5
#define _MODULES_R5_COMMANDS

#include <mem/memoryControl.h>
#include <boolean.h>
#include <core/comHandler.h>
#include <core/help.h>
#include <modules/R5/help.h>
#include <modules/mpx_supt.h>


/**
 * Registers the permanent commands in the command handler
 */
void registerR5TempCommands();


/**
 * Initializes the heap to the provided size and creates a free mem block across it
 *
 * @return True or false
 */
const char *initHeap(char **args, int numArgs);

/**
 * Allocates a memory block if enough memory is availabel
 *
 * @param size - size of memory to allocate in bytes
 * @return pointer to the me
 */
const char *allocateMem(char **args, int numArgs);

/**
 * Deallocates the block of memory at the mempointer
 *
 * @param memPointer - pointer to the mem block
 */

const char *freeMemory(char **args, int numArgs);

const char *isEmptyCom(char **args, int numArgs);

#endif