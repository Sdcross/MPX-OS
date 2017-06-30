#ifndef _R3_R3COMMANDS_H
#define _R3_R3COMMANDS_H

#include <modules/R3/commands/help.h>

/**
 * Registers commands in command handler
 */
void registerR3Commands();

/**
 * Yields command handler to allow other processes to run.
 *
 * Usage: yield
 *
 * Args:
 * 	[no args] - yields command handler
 *
 * @param args The arguments to pass to the function
 * @return ""
 */
const char *yield(char **args, int numArgs);

/**
 * Loads the r3 processes to the queue.\n")
 *
 * Usage: loadr3
 *
 * Args:
 * 	[no args] - loads processes
 *
 * @param args The arguments to pass to the function
 * @return ""
 */
const char *loadr3(char **args, int numArgs);

#endif
