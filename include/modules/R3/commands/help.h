#ifndef _MODULES_R3_COMMANDS_HELP_PERM_H
#define _MODULES_R3_COMMANDS_HELP_PERM_H

#define HELP_R3_COMMAND_YIELD ((const char*) \
	"Yields command handler to allow other processes to run.\n"\
	"\n"\
    "Usage: yield\n"\
    "\n"\
    "Args:\n"\
    "    [no args] - yields command handler\n")


#define HELP_R3_COMMAND_LOAD ((const char*) \
	"Loads the r3 processes to the queue.\n"\
	"\n"\
    "Usage: loadr3\n"\
    "\n"\
    "Args:\n"\
    "    [no args] - loads processes\n")

#endif
