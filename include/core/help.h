#ifndef _HELP_H
#define _HELP_H

#define HELP_UNKNOWN_COMMAND ((const char*) "Unknown command.")
#define HELP_INVALID_ARGUMENTS ((const char*) "Invalid arguments. Please check the documentation for this command.")

#define HELP_COMMAND_VERSION ((const char*) \
    "Prints the current version of the OS.\n"\
    "\n"\
    "Usage: version\n"\
    "\n"\
    "Args:\n"\
    "    [no args] - Returns the version")

#define HELP_COMMAND_HELP ((const char*) \
    "Prints help for the specified commands.\n"\
    "\n"\
    "Usage: help commandName\n"\
    "\n"\
    "Args:\n"\
    "    [no args] - Returns the help for the help command\n"\
    "    commandName - The name of the command to get help for")


#define HELP_COMMAND_SHUTDOWN ((const char*) \
    "Shuts down the OS after asking for confirmation.\n"\
    "\n"\
    "Usage: shutdown [--confirm]"\
    "\n"\
    "Args:\n"\
    "    [no args] - Displays confirmation prompt\n"\
    "    --confirm - Auto-confirms shutdown")

#define HELP_COMMAND_DATE ((const char*) \
    "Prints the current date/time in ISO-8601 format.\n"\
	"Improperly specified date/times are ignored.\n"\
    "\n"\
    "Usage: date [--date] [--time] [--setdate yyyy-MM-dd] [--settime hh:mm:ss]\n"\
    "\n"\
    "Args:\n"\
    "    [no args] - Return the date and time\n"\
    "    --time - Return the time\n"\
    "    --date - Return the date\n"\
    "    --setdate - Sets the date to the specified date (returns the new date/time)\n"\
    "    --settime - Sets the time to the specified time (returns the new date/time)")

#endif
