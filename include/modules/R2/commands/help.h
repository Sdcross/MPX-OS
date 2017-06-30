#ifndef _MODULES_R2_COMMANDS_HELP_PERM_H
#define _MODULES_R2_COMMANDS_HELP_PERM_H

#define HELP_R2_COMMAND_SPCB ((const char*) \
	"Places a PCB into the suspended state and reinserts it into the appropriate queue.\n"\
	"\n"\
	"Usage: spcb name\n"\
	"\n"\
	"Args:\n"\
	"    name - The name of the process to suspend (must exist)")

#define HELP_R2_COMMAND_RPCB ((const char*) \
	"Places a PCB into the not suspended state and reinserts it into the appropriate queue.\n"\
	"\n"\
	"Usage: rpcb name\n"\
	"\n"\
	"Args:\n"\
	"    name - The name of the process to resume (must exist)\n"\
	"    --all - Resumes all processes")

#define HELP_R2_COMMAND_PPCB ((const char*) \
	"Sets a PCB's priority and reinserts the process into the correct place in the correct queue.\n"\
	"\n"\
	"Usage: ppcb name priority\n"\
	"\n"\
	"Args:\n"\
	"    name - The name of the process to set the priority on (must exist)\n"\
	"    priority - The new priority (between 0 and 9)")

#define HELP_R2_COMMAND_SHOWPCB ((const char*) \
	"Displays the following information for the specified PCBs:\n"\
	"    Process Name:\n"\
	"    Class:\n"\
	"    State:\n"\
	"    Suspended Status:\n"\
	"    Priority:\n"\
	"\n"\
	"Usage: showpcb [--all] [--ready] [--blocked] [--name pcbName]\n"\
	"    (at least 1 must be specified)\n"\
	"\n"\
	"Args:\n"\
	"    [no args] - Shows the help for this command\n"\
	"    --all - Displays information for all PCBs\n"\
	"    --ready - Displays information for ready PCBs\n"\
	"    --blocked - Displays information for blocked PCBs\n"\
	"    --suspended - Displays information for suspended PCBs\n"\
	"    --name - Displays information for the specified PCB (can be used multiple times)")

#endif
