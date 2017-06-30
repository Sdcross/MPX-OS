#ifndef _CORE_COMMHANDLER
#define _CORE_COMMHANDLER

/*
  ----- comHandler.c -----

  Description..: Contains methods and variables used for
    command handler
*/

#include <stdint.h>
#include <string.h>

#include <core/io.h>
#include <core/serial.h>

typedef struct {
	char *name;
	const char *helpString;

	const char *(*funcPointer)(char **args, int numArgs);
} functionDef;

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
void addFunctionDef(char *name, const char *helpString, const char *(funcPointer)(char **args, int numArgs));

/**
 * Gets the functionDef struct corresponding to the name provided, returns a functionDef with null
 * funcPointer if none are found
 *
 * @param name - name of the functionDef
 * @return functionDef
 */
functionDef getFunctionDef(char *name);

/**
 * Gets the help string from the struct for the function name provided
 *
 * @param name - name associated witht he struct from which to get the help string
 * @return const char* help string
 */
const char *getHelpString(char *name);
/*******************************
 * Handle Com History
 *******************************/

/**
 * Helper function to get the next or previous command from the command history
 *
 * @param isPrev integer denoting if to get the previous command
 * @return string of the command
 */
char *getComHistory(int isPrev);

/**
 * Helper function to add a command to the command history array
 *
 * @param newCom string to add to the command history
 */
void addComHistory(char *newCom);

/*******************************
 * Helper Functions
 *******************************/
/**
 * Helper function to print out the beginning line tag: ">>"
 */
void printStart();

/**
 * Helper function to move the insertion point from the end of the line to the correct placement
 *
 * @param endIndex - index of last char printed
 * @param insertionIndex - index of where insertion point should be
 */
void returnToInsertionPoint(int endIndex, int insertionIndex);

/**
 * Helper function to remove all printed chars on the current line of input back to the >>
 *
 * @param endIndex - index of last char printed
 * @param insertionIndex - index of where insertion point should be
 */
void eraseCurrentRow(int endIndex, int insertionIndex);

/*******************************
 * Handle Input
 *******************************/

/**
 * Polls the input for characters and handles special key strokes such as delete, backspace, arrows, etc.
 * and returns the input string
 *
 * @return string that was input
 */
char *getInput();

/**
 * Gets the command in the given commandString param and executes it, printing the provided output string
 *
 * @param commandString string contianing the command name and any args
 */
void executeCommand(char *commandString);

/*******************************
 * Initialization / Main
 *******************************/
/**
 * Initialization function to add commands ot the functionDefs array
 *
 */
void setupCommands();

/**
 * Main function of the comHandler that initializes the command handler, continually loops taking in
 * input commands, manages the comHistory, and executes given commands
 */
void initCommandHandler();

#endif