#ifndef _QUEUE_H
#define _QUEUE_H

#include "pcb.h"
#include "boolean.h"

/**
 * The struct representing a node in a queue
 */
typedef struct node {
	struct pcb *data;
	struct node *next;
	struct node *prev;
} node;

/* Get queue functions */
/**
 * Gets the head node of the ready queue.
 *
 * @return The head node of the ready queue
 */
node *getReadyQueue();

/**
 * Gets the head node of the blocked queue.
 *
 * @return The head node of the blocked queue
 */
node *getBlockedQueue();

/**
 * Gets the head node of the suspended-ready queue.
 *
 * @return The head node of the suspended-ready queue
 */
node *getSuspendedReadyQueue();

/**
 * Gets the head node of the suspended-blocked queue.
 *
 * @return The head node of the suspended-blocked queue
 */
node *getSuspendedBlockedQueue();
/* End of get queue functions */

/* Pop queue functions */
/**
 * Pops the next node off of the ready queue.
 *
 * @return The next node of the ready queue, or NULL if it is empty
 */
pcb *popReady();

/**
 * Pops the next node off of the blocked queue.
 *
 * @return The next node of the blocked queue, or NULL if it is empty
 */
pcb *popBlocked();

/**
 * Pops the next node off of the suspended-ready queue.
 *
 * @return The next node of the suspended-ready queue, or NULL if it is empty
 */
pcb *popSuspendedReady();

/**
 * Pops the next node off of the suspended-blocked queue.
 *
 * @return The next node of the suspended-blocked queue, or NULL if it is empty
 */
pcb *popSuspendedBlocked();
/* End of pop queue functions */

/* Insert/Remove/Search queue functions */
/**
 * Inserts the PCB into the appropriate queue.
 *
 * @param p The PCB to insert.
 * @return true if the PCB was inserted, false otherwise
 */
boolean insertPCB(pcb *p);

/**
 * Removes the given PCB from it's queue.
 *
 * @param p The PCB to remove
 * @return true if the PCB was removed, false otherwise
 */
boolean removePCB(pcb *p);

/**
 * Finds the PCB with the given process name.
 *
 * @param processName The name of the process to search for
 * @return A pointer to the PCB, or null if not found
 */
pcb *findPCB(const char *processName);
/* End of Insert/Remove/Search queue functions */

#endif