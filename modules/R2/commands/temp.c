#include <core/comHandler.h>
#include <core/help.h>
#include <core/queue.h>
#include <modules/R2/commands/temp.h>

/**
 * Registers the temporary commands in the command handler
 */
void registerR2TempCommands(){
	addFunctionDef("cpcb", HELP_R2_COMMAND_CPCB, createPcb);
	addFunctionDef("dpcb", HELP_R2_COMMAND_DPCB, deletePcb);
	addFunctionDef("bpcb", HELP_R2_COMMAND_BPCB, blockPcb);
	addFunctionDef("upcb", HELP_R2_COMMAND_UPCB, unblockPcb);
}

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
const char *createPcb(char **args, int numArgs){
	if (numArgs != 3) {      //makes sure there is the proper number of arguments
		return HELP_INVALID_ARGUMENTS;
	}
	int isClassZero = strcmp(args[1], "0") == 0; //converts the string of the class to an int and validates input
	int class = atoi(args[1]);
	int isPriorityZero = strcmp(args[2], "0") == 0; //convert the string of the priority to an int and validates input
	int priority = atoi(args[2]);

	if (!checkParamName(args[0]) || 
	   (!checkParamClass(class) || (class==0 && !isClassZero)) ||
	   (!checkParamPriority(priority) || (priority==0 && !isPriorityZero)))
 	{ 
		return HELP_INVALID_ARGUMENTS;
	}
	pcb *p = findPCB(args[0]);
	if (p!=NULL) {	// Check if PCB name exists
		return PROCESS_NAME_ALREADY_EXISTS;
	} else {
		
		p = setupPCB(args[0], class, priority);  //checks for valid process name, process class, and priority, in setupPCB
	
		insertPCB(p);					// Inserts the PCB to it's new queue
	}

	return CREATE_PCB_SUCCESS;
}

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
const char *deletePcb(char **args, int numArgs){
	if (numArgs != 1) {     //makes sure there is the proper number of arguments
		return HELP_INVALID_ARGUMENTS;
	}
	pcb *p = findPCB(args[0]);
	if (p==NULL) {	// Check if PCB name exists
		return UNKNOWN_PCB_NAME;
	}
	else {

		removePCB(p); //Uses findPCB which returns a PCB pointer then plugs that into the removePCB
		freePCB(p); //Uses findPCB which returns a PCB pointer then plugs that into the freePCB
	}

	return DELETE_PCB_SUCCESS;
}

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
const char *blockPcb(char **args, int numArgs){

	if (numArgs != 1) {         //makes sure there is the proper number of arguments
		return HELP_INVALID_ARGUMENTS;
	}

	if (!checkParamName(args[0])) {   //checks for valid process name
		return HELP_INVALID_ARGUMENTS;
	}
	pcb *p = findPCB(args[0]);
	if (p==NULL) {	// Check if PCB name exists
		return UNKNOWN_PCB_NAME;
	} else {
		removePCB(p);		//removes PCB from the queue
		p-> state=BLOCKED;	//put the process in the block state
		insertPCB(p);		//inserts PCB into proper queue
	}

	return BLOCK_PCB_SUCCESS;
}

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
const char *unblockPcb(char **args, int numArgs){
	if (numArgs != 1) {                     //makes sure there is the proper number of arguments
		return HELP_INVALID_ARGUMENTS;
	}

	if (!checkParamName(args[0])) { //checks for valid process name
		return HELP_INVALID_ARGUMENTS;
	}
	pcb *p = findPCB(args[0]);
	if (p==NULL) {	// Check if PCB name exists
		return UNKNOWN_PCB_NAME;
	} else {
		
		removePCB(p); //removes PCB from the queue 
		p->state=READY;//put the unblock state on it
		insertPCB(p); //inserts PCB into proper queue
	}

	return UNBLOCK_PCB_SUCCESS;

}