#ifndef _MODULES_R2_COMMANDS_TEMP_H
#define _MODULES_R2_COMMANDS_TEMP_H

#include "help_temp.h"
#include "status.h"
#include "status_temp.h"

/**
 * Registers the temporary commands in the command handler
 */
void registerR2TempCommands();

/**
 * Creates a PCB and inserts it into the appropriate queue.
 *
 * Note: This command will be removed in module R3/R4
 *
 * Usage: cpcb name class priority
 *
 * Args:
 * 	name - The Process Name (must be unique)
 * 	class - The Process Class (either 0 (system) or 1 (application))
 * 	priority - The Process Priority (number between 0 and 9)
 *
 * @param args The arguments to pass to the function
 * @param numArgs The number of arguments
 * @return A status message indicating success/failure
 */
const char *createPcb(char **args, int numArgs);

/**
 * Removes a PCB from the appropriate queue and then frees all associated memory.
 *
 * Note: This command will be removed in module R3/R4
 *
 * Usage: dpcb name
 *
 * Args:
 * 	name - The Process Name to remove (must exist)
 *
 * @param args The arguments to pass to the function
 * @param numArgs The number of arguments
 * @return A status message indicating success/failure
 */
const char *deletePcb(char **args, int numArgs);

/**
 * Places a PCB into the blocked state and reinserts it into the appropriate queue.
 *
 * Note: This command will be removed in module R3/R4
 *
 * Usage: bpcb name
 *
 * Args:
 *	name - The Process Name to place into the blocked state (must exist)
 *
 * @param args The arguments to pass to the function
 * @param numArgs The number of arguments
 * @return A status message indicating success/failure
 */
const char *blockPcb(char **args, int numArgs);

/**
 * Places a PCB into the unblocked state and reinserts it into the appropriate queue.
 *
 * Note: This command will be removed in module R3/R4
 *
 * Usage: upcb name
 *
 * Args:
 * 	name - The Process Name to place into the unblocked state (must exist)
 *
 * @param args The arguments to pass to the function
 * @param numArgs The number of arguments
 * @return A status message indicating success/failure
 */
const char *unblockPcb(char **args, int numArgs);

#endif
