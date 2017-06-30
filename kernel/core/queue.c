#include <core/queue.h>
#include <modules/mpx_supt.h>
#include <string.h>

/* Internal Functions and Data Structures */
typedef enum {
	QUEUE_BLOCKED = BLOCKED,
	QUEUE_READY = READY,
	QUEUE_SUSPENDED_BLOCKED = BLOCKED + 0x02,
	QUEUE_SUSPENDED_READY = READY + 0x02
} queue;

node *_newNode(pcb *p);
node *_findNode(const char *processName);
node *_findNodeInQueue(queue q, const char *processName);

boolean _insertPriority(queue q, node *newNode);
boolean _insertFIFO(queue q, node *newNode);
/* Internal Functions and Data Structures */

/* Array of the heads of the queues. Indexes are defined in the queue enum */
node *queues[4];

/**
 * Internal function to create a new list node.
 *
 * @param pcb The pcb to store in the node
 * @return A pointer to the created node, or NULL if the node can't be created
 */
node *_newNode(pcb *pcb) {
	// Make sure the given pcb isn't null
	if (pcb == NULL) {
		// Nice try
		return NULL;
	}

	// Allocate memory for the node
	node *node = sys_alloc_mem(sizeof(struct node));	//TODO: Make sure memory allocates

	// Initiailize the node pointers
	node->data = pcb;
	node->next = NULL;
	node->prev = NULL;

	// Return the node
	return node;
}

/**
 * Internal function to find the node containing the PCB with the given process name.
 *
 * @param processName The process name to search for
 * @return The node containing the PCB with the given process name, or null if not found
 */
node *_findNode(const char *processName) {
	// Make sure the given name isn't null
	if (processName == NULL) {
		// Nice try
		return NULL;
	}

	node *n = NULL;

	// Search the ready queue for the process
	if (n == NULL) {
		n = _findNodeInQueue(QUEUE_READY, processName);
	}

	// Search the blocked queue for the process
	if (n == NULL) {
		n = _findNodeInQueue(QUEUE_BLOCKED, processName);
	}

	// Search the suspended-ready queue for the process
	if (n == NULL) {
		n = _findNodeInQueue(QUEUE_SUSPENDED_READY, processName);
	}

	// Search the suspended-blocked queue for the process
	if (n == NULL) {
		n = _findNodeInQueue(QUEUE_SUSPENDED_BLOCKED, processName);
	}

	return n;
}

/**
 * Internal function for finding a node in a specific queue
 *
 * @param q The queue to search in
 * @param processName The process name to search for
 * @return The node containing the PCB with the given name, or null if not found
 */
node *_findNodeInQueue(queue q, const char *processName) {
	// Make sure a name is given
	if (processName == NULL) {
		// Nice try
		return NULL;
	}

	node *curr = queues[q];

	// Keep looking through the list until the process name is found
	// curr == NULL if the process name isn't found
	while (curr != NULL && strcmp(curr->data->processName, processName) != 0) {
		curr = curr->next;
	}

	return curr;
}

/**
 * Internal function for inserting a node into a given queue in order by priority.
 *
 * @param q The queue to insert into
 * @param newNode The node to insert
 * @return true if the node was inserted, false otherwise
 */
boolean _insertPriority(queue q, node *newNode) {
	if (newNode == NULL) {
		// Nice try
		return false;
	}

	// Check if queue is empty, if it is, queue becomes the specified node
	if (queues[q] == NULL) {
		queues[q] = newNode;
		return true;
	}

	// The queue isn't empty
	node *curr = queues[q];

	// Get to the end of all priorities greater than the new node's priority
	while (curr->data->priority >= newNode->data->priority && curr->next != NULL) {
		curr = curr->next;
	}

	// Check if the new node should be the next element, but curr didn't
	// advance there because it would have been null
	if (curr->data->priority >= newNode->data->priority && curr->next == NULL) {
		// End of the queue
		curr->next = newNode;
		newNode->prev = curr;
	} else if (curr->data->priority < newNode->data->priority && curr->prev == NULL) {
		// We need to set a new head
		newNode->next = queues[q];
		queues[q]->prev = newNode;
		queues[q] = newNode;
	} else {
		// In the middle of 2 nodes
		newNode->next = curr;
		newNode->prev = curr->prev;
		curr->prev->next = newNode;
		curr->prev = newNode;
	}

	return true;
}

/**
 * Inserts a node into a FIFO queue
 *
 * @param q The queue to insert into
 * @param newNode The node to insert
 * @return true if the node was inserted, false otherwise
 */
boolean _insertFIFO(queue q, node *newNode) {
	if (newNode == NULL) {
		// Nice try
		return false;
	}

	// Check if queue is empty, if it is, queue becomes the specified node
	if (queues[q] == NULL) {
		queues[q] = newNode;
		return true;
	}

	// The queue isn't empty
	node *curr = queues[q];

	// Get to the end of the list
	while (curr->next != NULL) {
		curr = curr->next;
	}

	// Add the node to the queue
	curr->next = newNode;
	newNode->prev = curr;

	return true;
}

/**
 * Gets the head node of the ready queue.
 *
 * @return The head node of the ready queue
 */
node *getReadyQueue() {
	return queues[QUEUE_READY];
}

/**
 * Gets the head node of the blocked queue.
 *
 * @return The head node of the blocked queue
 */
node *getBlockedQueue() {
	return queues[QUEUE_BLOCKED];
}

/**
 * Gets the head node of the suspended-ready queue.
 *
 * @return The head node of the suspended-ready queue
 */
node *getSuspendedReadyQueue() {
	return queues[QUEUE_SUSPENDED_READY];
}

/**
 * Gets the head node of the suspended-blocked queue.
 *
 * @return The head node of the suspended-blocked queue
 */
node *getSuspendedBlockedQueue() {
	return queues[QUEUE_SUSPENDED_BLOCKED];
}

/**
 * Pops the next node off of the ready queue.
 *
 * @return The next node of the ready queue, or NULL if it is empty
 */
pcb *popReady() {
	if (queues[QUEUE_READY] == NULL) {
		// Queue is empty
		return NULL;
	}

	node *ready = queues[QUEUE_READY];
	pcb *ret = ready->data;

	// Advance ready
	ready = ready->next;
	sys_free_mem(ready->prev);
	ready->prev = NULL;

	// Update the head of the queue
	queues[QUEUE_READY] = ready;

	return ret;
}

/**
 * Pops the next node off of the blocked queue.
 *
 * @return The next node of the blocked queue, or NULL if it is empty
 */
pcb *popBlocked() {
	if (queues[QUEUE_BLOCKED] == NULL) {
		// Queue is empty
		return NULL;
	}

	node *blocked = queues[QUEUE_BLOCKED];
	pcb *ret = blocked->data;

	// Advance blocked
	blocked = blocked->next;
	sys_free_mem(blocked->prev);
	blocked->prev = NULL;

	// Update the head of the queue
	queues[QUEUE_BLOCKED] = blocked;

	return ret;
}

/**
 * Pops the next node off of the suspended-ready queue.
 *
 * @return The next node of the suspended-ready queue, or NULL if it is empty
 */
pcb *popSuspendedReady() {
	if (queues[QUEUE_SUSPENDED_READY] == NULL) {
		// Queue is empty
		return NULL;
	}

	node *suspendReady = queues[QUEUE_SUSPENDED_READY];
	pcb *ret = suspendReady->data;

	// Advance suspended ready
	suspendReady = suspendReady->next;
	sys_free_mem(suspendReady->prev);
	suspendReady->prev = NULL;

	// Update the head of the queue
	queues[QUEUE_SUSPENDED_READY] = suspendReady;

	return ret;
}

/**
 * Pops the next node off of the suspended-blocked queue.
 *
 * @return The next node of the suspended-blocked queue, or NULL if it is empty
 */
pcb *popSuspendedBlocked() {
	if (queues[QUEUE_SUSPENDED_BLOCKED] == NULL) {
		// Queue is empty
		return NULL;
	}

	node *suspendBlocked = queues[QUEUE_SUSPENDED_BLOCKED];
	pcb *ret = suspendBlocked->data;

	// Advance suspended blocked
	suspendBlocked = suspendBlocked->next;
	sys_free_mem(suspendBlocked->prev);
	suspendBlocked->prev = NULL;

	// Update the head of the queue
	queues[QUEUE_SUSPENDED_BLOCKED] = suspendBlocked;

	return ret;
}

/**
 * Inserts the PCB into the appropriate queue.
 *
 * @param p The PCB to insert.
 * @return true if the PCB was inserted, false otherwise
 */
boolean insertPCB(pcb *p) {
	if (p == NULL) {
		// Nice try
		return false;
	}

	// Allocate a new node
	node *node = _newNode(p);

	// Make sure memory actually allocated
	if (node == NULL) {
		return false;
	}

	if (p->state == READY) {
		if (p->isSuspended) {
			// Suspended ready
			_insertPriority(QUEUE_SUSPENDED_READY, node);
		} else {
			// Ready
			_insertPriority(QUEUE_READY, node);
		}
	} else if (p->state == BLOCKED) {
		if (p->isSuspended) {
			// Suspended blocked
			_insertFIFO(QUEUE_SUSPENDED_BLOCKED, node);
		} else {
			// Blocked
			_insertFIFO(QUEUE_BLOCKED, node);
		}
	} else {
		// Not in the ready or blocked state, not adding to a queue.
		return false;
	}

	return true;
}

/**
 * Removes the given PCB from it's queue.
 *
 * @param p The PCB to remove
 * @return true if the PCB was removed, false otherwise
 */
boolean removePCB(pcb *p) {
	if (p == NULL) {
		// Nice try
		return false;
	}

	// Find the node containing the pcb
	node *n = _findNode(p->processName);

	// Make sure the node was found
	if (n == NULL) {
		return false;
	}

	// Check if it was the head node
	if (n->prev != NULL) {
		n->prev->next = n->next;
	} else {
		// Hacky way of figuring out what queue the node was in
		queue q = (n->data->state | ((n->data->isSuspended << 1) & 0x02));
		queues[q] = n->next;
	}

	// Check if it was the tail node
	if (n->next != NULL) {
		n->next->prev = n->prev;
	}

	n->next = NULL;
	n->prev = NULL;

	sys_free_mem(n);

	return true;
}

/**
 * Finds the PCB with the given process name.
 *
 * @param processName The name of the process to search for
 * @return A pointer to the PCB, or null if not found
 */
pcb *findPCB(const char *processName) {
	if (processName == NULL) {
		// Nice try
		return NULL;
	}

	// Find the node containing the pcb with the given name
	node *n = _findNode(processName);

	// If nothing was found, return null, otherwise return the pcb
	return n == NULL ? NULL : n->data;
}
