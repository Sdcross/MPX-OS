#include <boolean.h>

#include <core/comHandler.h>
#include <core/help.h>
#include <core/queue.h>
#include <boolean.h>

#include <modules/R5/commands/r5commands.h>
#include <mem/memoryControl.h>

void printBlockInfo(cmcb *blockList);
void printCmcbInfo(cmcb *block);

/**
 * Registers the permanent commands in the command handler
 */
void registerR5PermCommands() {
	addFunctionDef("showMemory", HELP_R5_COMMAND_SHOWMEMORY, showMemory);
}

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
const char *showMemory(char **args, int numArgs) {
	if (numArgs == 0) {
		return HELP_R5_COMMAND_SHOWMEMORY;
	}

	boolean freeFlag = false;
	boolean allocFlag = false;

	int i;
	for (i = 0; i < numArgs; i++) {
		if (strcmp(args[i], "--all") == 0) {
			freeFlag = true;
			allocFlag = true;
			
		} else if (strcmp(args[i], "--free") == 0) {
			freeFlag = true;
		} else if (strcmp(args[i], "--allocated") == 0) {
			allocFlag = true;
		}
		
	}

	serial_print("\n");


	if (freeFlag) {
		cmcb *freeBlocks = getFreeHead();

		if (freeBlocks != NULL) {
			serial_print("\n");
			serial_println("Free Memory");
			serial_println("=======================");

			printBlockInfo(freeBlocks);
		}

	}

	if (allocFlag) {
		cmcb *allocatedBlocks = getAllocatedHead();

		if (allocatedBlocks != NULL) {
			serial_print("\n");
			serial_println("Allocated Memory");
			serial_println("=======================");

			printBlockInfo(allocatedBlocks);
		}

	
	}

	return "";
}
void printBlockInfo(cmcb *blockList) {
	if (blockList != NULL) {
		cmcb *current = blockList;

		while (current != NULL) {
			printCmcbInfo(current);

			serial_print("\n\n");			// Put 2 newlines between each one

			current = current->next;
		}
	}
}

void printCmcbInfo(cmcb *block) {
	char cmcbType[2];
	char cmcbAddr[50];
	char cmcbSize[11];
	char cmcbMemsize[11];

	itoa(block ->type, cmcbType, 10);
	itoa((int)block ->beginningAddr, cmcbAddr, 10);
	itoa(block ->size, cmcbSize, 10);
	itoa(block ->memSize, cmcbMemsize, 10);

	serial_print("CMCB Type: ");
	serial_println(cmcbType);
	serial_print("Begining Memory Address: ");
	serial_println(cmcbAddr);
	serial_print("Block Size: ");
	serial_println(cmcbSize);
	serial_print("Memory Size: ");
	serial_println(cmcbMemsize);
	serial_print("Process Name: ");
	serial_println(block -> name);
}
