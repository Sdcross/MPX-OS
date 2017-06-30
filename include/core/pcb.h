/*
  ----- pcb.h -----

  Description..: Contains methods and variables used for
    command handler
*/
#include <system.h>

#ifndef _PCB_H
#define _PCB_H

/* PCB states. Do not change these values as queues depend on these being these specific numbers. */
#define BLOCKED  0
#define READY  1
#define RUNNING 2


/* PCB process classes */
#define SYSTEM 0
#define APPLICATION 1

typedef struct pcb {
	char *processName;
	int processClass;
	int priority;

	int isSuspended;
	int state;
	//stack area, min 1024bytes
	unsigned char* stackTop;
	unsigned char* stackBottom;
} pcb;

/**
 * Allocates memory for a new PCB and returns a pointer to it
 *
 * @return PCB pointer or Null if error occurs
 */
pcb *allocatePCB();

/**
 * Frees memory that is allocated for the pcb provided
 *
 * @param pcbPtr pointer to pcb to be freed
 * @return integer code - 1 if successful, -1 otherwise
 */
int freePCB(pcb *pcbPtr);

/**
 * Allocates memory for a new PCB and sets it with given params
 *
 * @param processName - const string name
 * @param processClass - integer identifying as system or application process (0, 1)
 * @param priority - integer between 0 and 9 indicating priority
 * @return PCB pointer to new pcb or NULL if there were errors
 */
pcb *setupPCB(const char *processName, int processClass, int priority);

/**
 * Validates that the processName is valid
 *
 * @param processName - const char * processName
 * @return integer 0 or 1 if valid
 */
int checkParamName(const char *processName);

/**
 * Validates that the processClass is valid
 *
 * @param processClass - int
 * @return integer 0 or 1 if valid
 */
int checkParamClass(int processClass);

/**
 * Validates that the priority is valid
 *
 * @param priority - int
 * @return integer 0 or 1 if valid
 */
int checkParamPriority(int priority);

#endif
