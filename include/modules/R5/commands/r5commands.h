#ifndef _R5_R5COMMANDS_H 
#define _R5_R5COMMANDS_H

#include <modules/R5/commands/help.h>  

/**
 * Registers commands in command handler
 */
void registerR5PermCommands();

/**
 * Displays the following information for the specified CMCB's:
 * 	CMCB Type:
 *	Begining Memory Address:
 *	Block Size:
 *	Memory Size:
 *	Process Name:
 *
 * Usage: showMemory [--all] [--free] [--allocated] 
 *
 * Args:
 *	[no args] - Shows the help for this command
 *	--all - Displays both free and allocated memory
 *	--free - Displays free memory
 *	--allocated - Displays allocated memory
 *
 * @param args The arguments to pass to the function
 * @param numArgs The number of arguments
 * @return A status message indicating success/failure
 */
 
const char *showMemory(char **args, int numArgs);

#endif