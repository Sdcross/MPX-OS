#ifndef _MODULES_R2_COMMANDS_PERM_H //include these two but for R3
#define _MODULES_R2_COMMANDS_PERM_H

#include "help.h"
#include "status.h"

/**
 * Registers the permanent commands in the command handler
 */
void registerR2PermCommands();

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
const char *suspendPcb(char **args, int numArgs);

/**
 * Places a PCB into the not suspended state and reinserts it into the appropriate queue.
 *
 * Usage: rpcb name
 *
 * Args:
 * 	name - The name of the process to resume (must exist)
 *
 * @param args The arguments to pass to the function
 * @param numArgs The number of arguments
 * @return A status message indicating success/failure
 */
const char *resumePcb(char **args, int numArgs);

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
const char *setPriorityPcb(char **args, int numArgs);

/**
 * Displays the following information for the specified PCBs:
 * 	Process Name:
 *	Class:
 *	State:
 *	Suspended Status:
 *	Priority:
 *
 * Usage: showpcb [--all] [--ready] [--blocked] [--name pcbName]
 *	(at least 1 must be specified)
 *
 * Args:
 *	[no args] - Shows the help for this command
 *	--all - Displays information for all PCBs
 *	--ready - Displays information for ready PCBs
 *	--blocked - Displays information for blocked PCBs
 *	--suspended - Displays information for suspended PCBs
 *	--name - Displays information for the specified PCB (can be used multiple times)
 *
 * @param args The arguments to pass to the function
 * @param numArgs The number of arguments
 * @return A status message indicating success/failure
 */
const char *showPcbInfo(char **args, int numArgs);

#endif
