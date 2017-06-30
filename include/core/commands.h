#ifndef _COMMANDS_H
#define _COMMANDS_H

/**
 * Returns the current version of the OS.
 *
 * Usage: version
 *
 * Args:
 * 	[no args] - Returns the version
 *
 * @param args The arguments to pass to the function
 * @param numArgs The number of arguments
 * @return The version of the OS.
 */
const char *version(char **args, int numArgs);

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
const char *help(char **args, int numArgs);

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
const char *shutdown(char **args, int numArgs);

/**
 * Returns the current date/time in ISO-8601 format.
 * Improperly specified date/times are ignored.
 *
 * Usage: date [--date] [--time] [--setdate yyyy-MM-dd] [--settime hh:mm:ss]
 *
 * Args:
 * 	[no args] - Return the date and time
 * 	--date - Return the date
 * 	--time - Return the time
 *	--setdate - Sets the date to the specified date (returns the new date/time)
 *	--settime - Sets the time to the specified time (returns the new date/time)
 *
 * @param args The arguments to pass to the function
 * @param numArgs The number of arguments
 * @return The ISO-8601 formatted date
 */
const char *date(char **args, int numArgs);

#endif
