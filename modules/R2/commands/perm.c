#include <boolean.h>

#include <core/comHandler.h>
#include <core/help.h>
#include <core/queue.h>
#include <boolean.h>

#include <modules/R2/commands/perm.h>

void printQueueInfo(node *queue);
void printPcbInfo(pcb *p);

/**
 * Registers the permanent commands in the command handler
 */
void registerR2PermCommands() {
	addFunctionDef("spcb", HELP_R2_COMMAND_SPCB, suspendPcb);
	addFunctionDef("rpcb", HELP_R2_COMMAND_RPCB, resumePcb);
	addFunctionDef("ppcb", HELP_R2_COMMAND_PPCB, setPriorityPcb);
	addFunctionDef("showpcb", HELP_R2_COMMAND_SHOWPCB, showPcbInfo);
}

/**
 * Places a PCB into the suspended state and reinserts it into the appropriate queue.
 *
 * Usage: spcb name
 *
 * Args:
 * 	name - The name of the process to suspend (must exist)
 *  --all - Resumes all processes
 *
 * @param args The arguments to pass to the function
 * @param numArgs The number of arguments
 * @return A status message indicating success/failure
 */
const char *suspendPcb(char **args, int numArgs) {
	if (numArgs != 1) {
		return HELP_INVALID_ARGUMENTS;
	}

	pcb *p = findPCB(args[0]);

	if (p == NULL) {					// Check if PCB name exists
		return UNKNOWN_PCB_NAME;
	} else {
		removePCB(p);					// Remove the PCB from it's current queue
		p->isSuspended = 1;				// Sets the suspended attribute to true
		insertPCB(p);					// Inserts the PCB to it's new queue
	}

	return SUSPEND_PCB_SUCCESS;
}

/**
 * Places a PCB into the not suspended state and reinserts it into the appropriate queue.
 *
 * Usage: rpcb name
 *
 * Args:
 * 	name - The name of the process to resume (must exist)
 *  --all - Resumes all processes
 *
 * @param args The arguments to pass to the function
 * @param numArgs The number of arguments
 * @return A status message indicating success/failure
 */
const char *resumePcb(char **args, int numArgs) {
	if (numArgs != 1) {
		return HELP_INVALID_ARGUMENTS;
	}

	if (strcmp(args[0], "--all")==0) {
		boolean isDone = false;
		while (isDone == false){
			pcb *susp = popSuspendedReady();
			if (susp == NULL){
				isDone = true;
			}
			else {
				susp->isSuspended = 0;
				insertPCB(susp);
			}
		}
		isDone = false;
		while (isDone == false){
			pcb *susp = popSuspendedBlocked();
			if (susp == NULL){
				isDone = true;
			}
			else {
				susp->isSuspended = 0;
				insertPCB(susp);
			}
		}
		return RESUME_PCBS_SUCCESS;
	}
	else {
		pcb *p = findPCB(args[0]);

		if (p == NULL) {                    // Check if PCB name exists
			return UNKNOWN_PCB_NAME;
		} else {
			removePCB(p);                    // Remove the PCB from it's current queue
			p->isSuspended = 0;                // Sets the suspended attribute to true
			insertPCB(p);                    // Inserts the PCB to it's new queue
		}

		return RESUME_PCB_SUCCESS;
	}
	}

/**
 * Sets a PCB's priority  and reinserts the process into the correct place in the correct queue.
 *
 * Usage: ppcb name priority
 *
 * Args:
 * 	name - The name of the process to set the priority on (must exist)
 * 	priority - The new priority (between 0 and 9)
 *
 * @param args The arguments to pass to the function
 * @param numArgs The number of arguments
 * @return A status message indicating success/failure
 */
const char *setPriorityPcb(char **args, int numArgs) {
	if (numArgs != 2) {
		return HELP_INVALID_ARGUMENTS;
	}

	int isZero = strcmp(args[1], "0") == 0;
	int priority = atoi(args[1]);
	pcb *p = findPCB(args[0]);

	if (p == NULL) {				// Validate PCB name
		return UNKNOWN_PCB_NAME;
	}

	if (!checkParamPriority(priority) || (priority == 0 && !isZero)) { // Validate priority
		return HELP_INVALID_ARGUMENTS;
	}

	removePCB(p);					// Remove the PCB from it's current queue
	p->priority = priority;			// Sets the suspended attribute to true
	insertPCB(p);					// Inserts the PCB to it's new queue

	return UPDATE_PRIORITY_SUCCESS;
}

/**
 * Displays the following information for the specified PCBs:
 * 	Process Name:
 *	Class:
 *	State:
 *	Suspended Status:
 *	Priority:
 *
 * Usage: showpcb [--all] [--ready] [--blocked] [--suspended] [--name pcbName]
 *
 * Args:
 *	[no args] - Shows the help for this command
 *	--all - Displays information for all PCBs
 *	--ready - Displays information for ready PCBs
 *	--blocked - Displays information for blocked PCBs
 *  --suspended - Displays information for suspended PCBs
 *	--name - Displays information for the specified PCB
 *
 * @param args The arguments to pass to the function
 * @param numArgs The number of arguments
 * @return A status message indicating success/failure
 */
const char *showPcbInfo(char **args, int numArgs) {
	if (numArgs == 0) {
		return HELP_R2_COMMAND_SHOWPCB;
	}

	boolean readyFlag = false;
	boolean blockedFlag = false;
	boolean suspendedFlag = false;
	pcb *p = NULL;

	int i;
	for (i = 0; i < numArgs; i++) {
		if (strcmp(args[i], "--all") == 0) {
			readyFlag = true;
			blockedFlag = true;
			suspendedFlag = true;
		} else if (strcmp(args[i], "--ready") == 0) {
			readyFlag = true;
		} else if (strcmp(args[i], "--blocked") == 0) {
			blockedFlag = true;
		} else if(strcmp(args[i], "--suspended") == 0) {
			suspendedFlag = true;
		} else if (strcmp(args[i], "--name") == 0) {
			if (numArgs < i + 1 && args[i + 1][0] != '-') {		// Make sure a process name is given, otherwise ignore
				p = findPCB(args[i + 1]);
			}
		}
	}

	serial_print("\n");

	if (p != NULL) {
		printPcbInfo(p);
		serial_print("\n\n");					// Put 2 newlines between each one
	}

	if (readyFlag) {
		node *queue = getReadyQueue();

		if (queue != NULL) {
			serial_print("\n");
			serial_println("Ready Queue");
			serial_println("=======================");

			printQueueInfo(queue);
		}

		if (suspendedFlag) {
			node *queue = getSuspendedReadyQueue();

			if (queue != NULL) {
				serial_print("\n");
				serial_println("Suspended-Ready Queue");
				serial_println("=======================");

				printQueueInfo(queue);
			}
		}
	}

	if (blockedFlag) {
		node *queue = getBlockedQueue();

		if (queue != NULL) {
			serial_print("\n");
			serial_println("Blocked Queue");
			serial_println("=======================");

			printQueueInfo(queue);
		}

		if (suspendedFlag) {
			node *queue = getSuspendedBlockedQueue();

			if (queue != NULL) {
				serial_print("\n");
				serial_println("Suspended-Blocked Queue");
				serial_println("=======================");


				printQueueInfo(queue);
			}
		}
	}

	return "";
}

void printQueueInfo(node *queue) {
	if (queue != NULL) {
		node *current = queue;

		while (current != NULL) {
			printPcbInfo(current->data);

			serial_print("\n\n");			// Put 2 newlines between each one

			current = current->next;
		}
	}
}

void printPcbInfo(pcb *p) {
	char processClass[2];
	char priority[2];
	char isSuspended[2];
	char state[2];

	itoa(p->processClass, processClass, 10);
	itoa(p->priority, priority, 10);
	itoa(p->isSuspended, isSuspended, 10);
	itoa(p->state, state, 10);

	serial_print("Process name: ");
	serial_println(p->processName);
	serial_print("Class: ");
	serial_println(processClass);
	serial_print("State: ");
	serial_println(state);
	serial_print("Suspended Status: ");
	serial_println(isSuspended);
	serial_print("Priority: ");
	serial_println(priority);
}