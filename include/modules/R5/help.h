#ifndef _MODULES_R5_COMMANDS_HELP //include these two but for R5
#define _MODULES_R5_COMMANDS_HELP

#define HELP_R5_COMMAND_HEAP ((const char*) \
	"Initializes the heap\n"\
	"\n"\
	"Usage: initHeap size\n"\
	"\n"\
	"Args:\n"\
	"    size - The size of the heap in bytes ")

#define HELP_R5_COMMAND_ALLOC ((const char*) \
	"Allocates memory block if memory is available\n"\
	"\n"\
	"Usage: allocMem size\n"\
	"\n"\
	"Args:\n"\
	"    size - The size of the memory in bytes")

#define HELP_R5_COMMAND_FREE ((const char*) \
	"Free up memory the memory at the given address\n"\
	"\n"\
	"Usage: freeMem address\n"\
	"\n"\
	"Args:\n"\
	"    address - the integer address of the memory")

#define HELP_R5_COMMAND_EMPTY ((const char*) \
	"Checks if memory is empty\n"\
	"\n"\
	"Usage: isEmpty \n"\
	"\n"\
	"Args:\n"\
	"    ")

#endif