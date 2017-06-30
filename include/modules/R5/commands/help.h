#ifndef _MODULES_R5_COMMANDS_HELP_PERM_H
#define _MODULES_R5_COMMANDS_HELP_PERM_H

#define HELP_R5_COMMAND_SHOWMEMORY ((const char*) \
	"Displays the following information for the specified CMCB's:\n"\
	"CMCB Type:\n"\
	"Begining Memory Address:\n"\
	"Block Size:\n"\
	"Memory Size:\n"\
	"Process Name:\n"\
	"\n"\
    "Usage: showMemory [--all] [--free] [--allocated]\n"\
    "\n"\
    "Args:\n"\
    "    [no args] - Shows the help for this command\n"\
    "    --all - Displays both free and allocated memory\n"\
    "    --free - Displays free memory\n"\
    "    --allocated - Displays allocated memory")




#endif