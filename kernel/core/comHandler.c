/*
  ----- comHandler.c -----

  Description..: Contains methods and variables used for
    command handler
*/

#include <stdint.h>
#include <string.h>

#include <core/comHandler.h>
#include <core/io.h>
#include <core/serial.h>
#include <core/help.h>
#include <core/commands.h>
#include <core/queue.h>

#include <modules/R2/commands/temp.h>
#include <modules/R2/commands/perm.h>
#include <modules/R3/commands/r3commands.h>
#include <modules/R5/commands/r5commands.h>
#include <modules/R5/memCommands.h>

#include <modules/mpx_supt.h>

int continueHandle = 1;
char buffer[256]; //buffer used for string input from keyboard

functionDef functionDefs[256]; //array of structs containing name and pointer to functions
int functionInsertPoint = 0;

char comHistory[10][256]; //Array containing 10 previously used commands
int comHistoryPos = 0; //Current position in comHistory

/*******************************
 * FunctionDef Handling
 *******************************/
/**
 * Adds function definition struct, created from provided params to the functionDefs array
 * This allows the function to be called in the command handler by its name
 *
 * @param name - string representation of the function
 * @param helpString - const string to be displayed for help
 * @param funcPointer - pointer to the function, must return const char* and take in arguments: char** args and int numArgs
 */
void addFunctionDef(char *name, const char *helpString, const char *(funcPointer)(char **args, int numArgs)) {
	if (functionInsertPoint == 256) { //check if array full
		serial_println("");
		serial_println("WARNING: Attempted to add more functions than allowed");
		return;
	}
	functionDef temp; //create new functionDef
	temp.name = name; //set all fields
	temp.funcPointer = funcPointer;
	temp.helpString = helpString;
	functionDefs[functionInsertPoint] = temp; //set functionDef in array
	functionInsertPoint++; //increment insertPoint
}

/**
 * Gets the functionDef struct corresponding to the name provided, returns a functionDef with null
 * funcPointer if none are found
 *
 * @param name - name of the functionDef
 * @return functionDef
 */
functionDef getFunctionDef(char *name) {
	int i = 0;
	for (; i < functionInsertPoint; i++) { //loop through functionDefs
		if (strcmp(functionDefs[i].name, name) == 0) { //check for correct name
			return functionDefs[i]; //if name correct return functionDef
		}
	}
	functionDef nullDef; //create blank functionDef
	nullDef.funcPointer = NULL;
	return nullDef;
}

/**
 * Gets the help string from the struct for the function name provided
 *
 * @param name - name associated witht he struct from which to get the help string
 * @return const char* help string
 */
const char *getHelpString(char *name) {
	int i = 0;
	for (; i < functionInsertPoint; i++) { //loop through functionDefs
		if (strcmp(functionDefs[i].name, name) == 0) { //if name correct
			return functionDefs[i].helpString; //return helpstring for functionDef
		}
	}
	return HELP_INVALID_ARGUMENTS; //return unkown command string
}
/*******************************
 * Handle Com History
 *******************************/

/**
 * Helper function to get the next or previous command from the command history
 *
 * @param isPrev integer denoting if to get the previous command
 * @return string of the command
 */
char *getComHistory(int isPrev) {
	if (isPrev == 1) { //increment or decrement position
		comHistoryPos--;
	} else {
		comHistoryPos++;
	}
	comHistoryPos = comHistoryPos % 10; //make sure within bounds
	return &comHistory[comHistoryPos][0]; //returns pointer to beginning of string
}

/**
 * Helper function to add a command to the command history array
 *
 * @param newCom string to add to the command history
 */
void addComHistory(char *newCom) {
	strcpy(&comHistory[comHistoryPos][0], newCom); //copy command into history
	comHistoryPos++; //increment position
	if (comHistoryPos == 10) { //if comHistory out of bounds return it within
		comHistoryPos = 0;
	}
}

/*******************************
 * Helper Functions
 *******************************/
/**
 * Helper function to print out the beginning line tag: ">>"
 */
void printStart() {
	serial_print("\n>> ");
}

/**
 * Helper function to move the insertion point from the end of the line to the correct placement
 *
 * @param endIndex - index of last char printed
 * @param insertionIndex - index of where insertion point should be
 */
void returnToInsertionPoint(int endIndex, int insertionIndex) {
	if (endIndex == NULL || insertionIndex == NULL){
		return;
	}

	int i = 0;
	for (; i < (endIndex - insertionIndex); i++) { //moves insertion point to the left to insertion point
		serial_print("\b"); // \b moves insertion point back 1 space
	}
}

/**
 * Helper function to remove all printed chars on the current line of input back to the >>
 *
 * @param endIndex - index of last char printed
 * @param insertionIndex - index of where insertion point should be
 */
void eraseCurrentRow(int endIndex, int insertionIndex) {
	if (endIndex == NULL || insertionIndex == NULL){
		return;
	}

	int i = 0;
	for (; i < (endIndex - insertionIndex); i++) { //moves insertion point right to end of line
		serial_print("\033[C"); //char moves insertion point right 1 line
	}
	for (i = 0; i < endIndex; i++) { //backspaces all chars on the line
		serial_print("\b \b"); //replaces char with space
	}
}

/*******************************
 * Handle Input
 *******************************/

/**
 * Polls the input for characters and handles special key strokes such as delete, backspace, arrows, etc.
 * and returns the input string
 *
 * @return string that was input
 */
char *getInput() {
	int insertPos = 0;
	int endPos = 0;
	int i = 0;
	int continueInput = 1;
	buffer[0] = '\0';

	set_serial_in(COM1);
	while (continueInput == 1) {
		if (inb(COM1 + 5) & 1) { //check if there is a char input
			char in = inb(COM1); //read char to in

			switch (in) {
				case 10: //carriage return /r
					for (i = 0; i < insertPos; i++) { //moves insertion point to far left of line
						serial_print("\033[D");
					}
					insertPos = 0;
					break;
				case 13: //enter /n
					continueInput = 0; //ends input loop

					// Print a newline when enter is pressed
					serial_print("\n");
					break;
				case 27: //arrow key
					inb(COM1); //useless bracket char
					in = inb(COM1); //arrow key char
					switch (in) {
						case 'A': // up
							eraseCurrentRow(endPos, insertPos); //erase current row
							strcpy(buffer, getComHistory(1)); //get previous command, copy into buffer
							endPos = strlen(buffer); //set endPos and insertPos to end of command
							insertPos = endPos;
							serial_print(buffer); //print buffer
							break;
						case 'B': // down
							eraseCurrentRow(endPos, insertPos); //erase current row
							strcpy(buffer, getComHistory(0)); //get next command, copy into buffer
							endPos = strlen(buffer); //set endPos and insertPos to length of buffer
							insertPos = endPos;
							serial_print(buffer); //print buffer
							break;
						case 'C': // right
							if (insertPos < endPos) { //cant move right if at end of line
								insertPos++;
								serial_print("\033[C"); //print string to move right 1 position
							}
							break;
						case 'D': //left
							if (insertPos > 0) { //cant move left if at beginning
								insertPos--;
								serial_print("\033[D"); //print string to move left 1 position
							}
							break;
					}
					break;
				case 127: //backspace
					if (insertPos == 0) { //cant backspace
						break;
					}
					eraseCurrentRow(endPos, insertPos); //clear row

					for (i = insertPos - 1; i < endPos + 1; i++) { //shift chars to the left
						buffer[i] = buffer[i + 1];
					}

					insertPos--;
					endPos--;
					serial_print(buffer); //print new buffer
					returnToInsertionPoint(endPos, insertPos); //move insertion point to correct placement
					break;
				case 126: //delete
					if (insertPos == endPos) { //cant delete at end of line
						break;
					}
					eraseCurrentRow(endPos, insertPos); //clear row

					for (i = insertPos; i < endPos + 1; i++) { //shift everything left erasing deleted char
						buffer[i] = buffer[i + 1];
					}
					endPos--;
					serial_print(buffer); //print new buffer
					returnToInsertionPoint(endPos, insertPos); //return buffer to correct position
					break;

				default:
					if (insertPos == endPos) { //insert to end
						buffer[insertPos++] = in; //reads char into buffer
						buffer[++endPos] = '\0'; //increment position and insert str end tag
						serial_print(&buffer[insertPos - 1]); //print last char added to screen
					} else {
						eraseCurrentRow(endPos, insertPos); //clears current row

						for (i = endPos + 1; i > insertPos; i--) { //shift chars to the right one
							buffer[i] = buffer[i - 1];
						}

						buffer[insertPos] = in; //insert new char
						serial_print(buffer); //print new buffer
						endPos++;
						insertPos++;
						returnToInsertionPoint(endPos, insertPos); //returns insert point to correct position
					}
					break;
			}
		}
	}
	return buffer;
}

/**
 * Gets the command in the given commandString param and executes it, printing the provided output string
 *
 * @param commandString string contianing the command name and any args
 */
void executeCommand(char *commandString) {
	if (strlen(commandString) == 0 || commandString == NULL) { //check if empty command
		return;
	}
	char *comName = strtok(commandString, " "); //get first token which will be command name

	functionDef funcDef;
	funcDef = getFunctionDef(comName); //get functionDef associated with command
	if (funcDef.funcPointer == NULL) { //check if was given blank functiondef, meaning no function for name
		serial_print("\nInvalid Function Name: ");
		serial_print(comName);
		return;
	}

	char *arrOfArgs[256]; //array of arg strings
	int continueRead = 1;
	int count = 0; //count of args
	while (continueRead == 1) {
		char *temp = strtok(NULL, " "); //strtok has arg string already and will return each one in sequence until end
		if (temp == NULL) { //temp will be null when out of tokens
			continueRead = 0;
		} else {
			arrOfArgs[count] = temp; //save arg
			count++;
		}
	}

	const char *respString = funcDef.funcPointer(arrOfArgs, count); //call functionPointer with args and count
	serial_println("");
	serial_print(respString); //print response
}

/*******************************
 * Locally definated commands
 *******************************/
/**
 * Returns help for the specified commands.
 *
 * Usage: help commandName
 *
 * Args:
 * 	[no args] - Returns the help for the help command
 * 	commandName - The name of the command to get help for
 *
 * @param args The arguments to pass to the function
 * @param numArgs The number of arguments
 * @return The help string
 */
const char *help(char **args, int numArgs) {
	if (numArgs > 0) { //make sure we have args
		const char *helpMessage = getHelpString(args[0]); //get help string
		if (helpMessage != NULL) {
			return helpMessage;
		}
	}
	int i = 0;
	serial_println("\nCommands List:");
	for(;i<functionInsertPoint;i+=1){
		serial_println(functionDefs[i].name);
	}
	serial_print("\nHelp: ");
	return HELP_COMMAND_HELP; //return help string for help function
}

/**
 * Shuts down the OS after asking for confirmation.
 *
 * Usage: shutdown [--confirm]
 *
 * Args:
 * 	[no args] - Displays confirmation prompt
 * 	--confirm - Auto-confirms shutdown
 *
 * @param args The arguments to pass to the function
 * @param numArgs The number of arguments
 * @return True if shutdown was confirmed
 */
const char *shutdown(char **args, int numArgs) {
	char buff[2] = {'a', '\0'}; //buffer string
	if (numArgs > 0) {
		if (strcmp(args[0], "--confirm") == 0) { //skips check
			continueHandle = 0;
			return "shutting down";
		}
	}
	serial_print("\nAre you sure? (y/n)"); //print check

	int getInp = 1;
	set_serial_in(COM1); //set serial input port
	while (getInp == 1) {
		if (inb(COM1 + 5) & 1) { //check if there is a char input
			char in = inb(COM1); //read char to in
			buff[0] = in; //set buff

			if (strcmp(buff, "y") == 0) { //if yes
				continueHandle = 0; //quit input loop
				return "shutting down";
			} else if (strcmp(buff, "n") == 0) { //continue loop
				return "not shutting down";
			} else {
				serial_println("\nInvalid input, please input (y/n)");
			}
		}
	}
	return "Erroneous Exit";
}

/*******************************
 * Initialization / Main
 *******************************/
/**
 * Initialization function to add commands ot the functionDefs array
 */
void setupCommands() {
	addFunctionDef("version", HELP_COMMAND_VERSION, version); //adds version
	addFunctionDef("help", HELP_COMMAND_HELP, help); //adds help
	addFunctionDef("shutdown", HELP_COMMAND_SHUTDOWN, shutdown); //adds shutdown
	addFunctionDef("date", HELP_COMMAND_DATE, date); //adds date

	// registerR2TempCommands(); - No need for these any more.
	registerR2PermCommands();
	registerR3Commands();
	// registerR5TempCommands(); - No need for these any more.
	registerR5PermCommands();
}

/**
 * Main function of the comHandler that initializes the command handler, continually loops taking in
 * input commands, manages the comHistory, and executes given commands
 */
void initCommandHandler() {
	setupCommands();
	while (continueHandle == 1) {
		printStart();
		char *command = getInput();
		addComHistory(command);
		executeCommand(command);

		// Put a new line after each command.
		serial_print("\n");

		// Let other processes execute.
		sys_req(IDLE);
	}

	// Remove the idle process.
	removePCB(findPCB("Idle"));

	sys_req(EXIT);
}