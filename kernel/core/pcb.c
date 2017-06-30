/*
  ----- pcb.c -----

  Description..: Contains methods and variables used for
    the PCB (process control blocks)
*/

#include <stdint.h>
#include <string.h>
#include <modules/mpx_supt.h>
#include <system.h>
#include <core/pcb.h>

/**
 * Allocates memory for a new PCB and returns a pointer to it
 *
 * freePCB should be used when done using the pcb to free the memory in use
 *
 * @return PCB pointer or Null if error occurs
 */
pcb *allocatePCB() {
	pcb *newPCB = NULL;
	newPCB = sys_alloc_mem(sizeof(struct pcb)); //malloc
	newPCB->processName = sys_alloc_mem(sizeof(char) * 256);
	newPCB->stackBottom = sys_alloc_mem(sizeof(unsigned char) * 4096);
	newPCB->stackTop = newPCB->stackBottom + (sizeof(unsigned char) * 4096) - sizeof(struct context);
	return newPCB;
}

/**
 * Frees memory that is allocated for the pcb provided
 *
 * @param pcbPtr pointer to pcb to be freed
 * @return integer code - 1 if successful, 0 otherwise
 */
int freePCB(pcb *pcbPtr) {
	sys_free_mem(pcbPtr->stackTop);
	sys_free_mem(pcbPtr->stackBottom);
	sys_free_mem(pcbPtr->processName);
	int ret = sys_free_mem(pcbPtr); //TODO Mod5 confirm works
	if (ret != -1) { //if returns value not -1 memory was released
		return 1;
	}
	return 0; //failed
}

/**
 * Allocates memory for a new PCB and sets it with given params
 *
 * @param processName - const string name
 * @param processClass - integer identifying as system or application process (0, 1)
 * @param priority - integer between 0 and 9 indicating priority
 * @return PCB pointer to new pcb or NULL if there were errors
 */
pcb *setupPCB(const char *processName, int processClass, int priority) {
	if (checkParamName(processName) == 0 || checkParamClass(processClass) == 0 || checkParamPriority(priority) == 0) {
		return NULL;
	}
	pcb *newPCB = allocatePCB(); //allocate mem
	strcpy(newPCB->processName, processName); //set values
	newPCB->processClass = processClass;
	newPCB->priority = priority;

	newPCB->isSuspended = 0; //set to defaults
	newPCB->state = READY;

	return newPCB;
}

/**
 * Validates that the processName is valid
 *
 * @param processName - const char * processName
 * @return integer 0 or 1 if valid
 */
int checkParamName(const char *processName) {
	if (processName == NULL) {
		return 0;
	}
	if (strlen(processName) == 0) {
		return 0;
	}
	return 1;
}

/**
 * Validates that the processClass is valid
 *
 * @param processClass - int
 * @return integer 0 or 1 if valid
 */
int checkParamClass(int processClass){
	if (processClass < 0 || processClass > 1) {
		return 0;
	}
	return 1;
}

/**
 * Validates that the priority is valid
 *
 * @param priority - int
 * @return integer 0 or 1 if valid
 */
int checkParamPriority(int priority){
	if (priority < 0 || priority > 9) {
		return 0;
	}
	return 1;
}
