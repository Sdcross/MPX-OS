#ifndef _MODULES_R2_COMMANDS_HELP_TEMP_H
#define _MODULES_R2_COMMANDS_HELP_TEMP_H

#define HELP_R2_COMMAND_CPCB ((const char*) \
	"Creates a PCB and inserts it into the appropriate queue.\n"\
	"\n"\
	"Note: This command will be removed in module R3/R4\n"\
	"\n"\
	"Usage: cpcb name class priority\n"\
	"\n"\
	"Args:\n"\
	"    name - The Process Name (must be unique)\n"\
	"    class - The Process Class (either 0 (system) or 1 (application))\n"\
	"    priority - The Process Priority (number between 0 and 9)")

#define HELP_R2_COMMAND_DPCB ((const char*) \
	"Removes a PCB from the appropriate queue and then frees all associated memory.\n"\
	"\n"\
	"Note: This command will be removed in module R3/R4\n"\
	"\n"\
	"Usage: dpcb name\n"\
	"\n"\
	"Args:\n"\
	"    name - The Process Name to remove (must exist)")

#define HELP_R2_COMMAND_BPCB ((const char*) \
	"Places a PCB into the blocked state and reinserts it into the appropriate queue.\n"\
	"\n"\
	"Note: This command will be removed in module R3/R4\n"\
	"\n"\
	"Usage: bpcb name\n"\
	"\n"\
	"Args:\n"\
	"    name - The Process Name to place into the blocked state (must exist)")

#define HELP_R2_COMMAND_UPCB ((const char*) \
	"Places a PCB into the unblocked state and reinserts it into the appropriate queue.\n"\
	"\n"\
	"Note: This command will be removed in module R3/R4\n"\
	"\n"\
	"Usage: upcb name\n"\
	"\n"\
	"Args:\n"\
	"    name - The Process Name to place into the unblocked state (must exist)")

#endif
