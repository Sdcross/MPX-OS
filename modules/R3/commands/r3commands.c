#include <system.h>
#include <boolean.h>
#include <core/queue.h>
#include <core/comHandler.h>
#include <modules/R3/processes.h>
#include <modules/R3/commands/r3commands.h>
#include <modules/mpx_supt.h>

#define P1_NAME ((const char*) "r3p1")
#define P2_NAME ((const char*) "r3p2")
#define P3_NAME ((const char*) "r3p3")
#define P4_NAME ((const char*) "r3p4")
#define P5_NAME ((const char*) "r3p5")

/**
 * Registers commands in command handler
 */
void registerR3Commands() {
	// addFunctionDef("yield", HELP_R3_COMMAND_YIELD, yield); - Removed for R4
	addFunctionDef("loadr3", HELP_R3_COMMAND_LOAD, loadr3);
}

/**
 * Yields command handler to allow other processes to run.
 *
 * Usage: yield
 *
 * Args:
 * 	[no args] - yields command handler
 *
 * @param args The arguments to pass to the function
 * @return ""
 */
const char *yield(char **args, int numArgs) {
	no_warn(args);
	no_warn(numArgs);

	asm volatile ("int $60");

	return "";
}

/**
 * Loads the r3 processes to the queue.\n")
 *
 * Usage: loadr3
 *
 * Args:
 * 	[no args] - loads processes
 *
 * @param args The arguments to pass to the function
 * @return ""
 */
const char *loadr3(char **args, int numArgs) {
	no_warn(args);
	no_warn(numArgs);

	boolean p1exists = findPCB(P1_NAME) != NULL ? true : false;
	boolean p2exists = findPCB(P2_NAME) != NULL ? true : false;
	boolean p3exists = findPCB(P3_NAME) != NULL ? true : false;
	boolean p4exists = findPCB(P4_NAME) != NULL ? true : false;
	boolean p5exists = findPCB(P5_NAME) != NULL ? true : false;

	if (!p1exists) {
		pcb *r3pcb1 = setupPCB(P1_NAME, 1, 1);
		r3pcb1->isSuspended = 1;
		context *cp1 = (context *)(r3pcb1->stackTop);
		memset(cp1, 0, sizeof(struct context));
		cp1->fs = 0x10;
		cp1->gs = 0x10;
		cp1->ds = 0x10;
		cp1->es = 0x10;
		cp1->cs = 0x8;
		cp1->ebp = (u32int)(r3pcb1->stackBottom);
		cp1->esp = (u32int)(r3pcb1->stackTop);
		cp1->eip = (u32int) proc1;
		cp1->eflags = 0x202;

		insertPCB(r3pcb1);
	}

	if (!p2exists) {
		pcb *r3pcb2 = setupPCB(P2_NAME, 1, 1);
		r3pcb2->isSuspended = 1;
		context *cp2 = (context *)(r3pcb2->stackTop);
		memset(cp2, 0, sizeof(struct context));
		cp2->fs = 0x10;
		cp2->gs = 0x10;
		cp2->ds = 0x10;
		cp2->es = 0x10;
		cp2->cs = 0x8;
		cp2->ebp = (u32int)(r3pcb2->stackBottom);
		cp2->esp = (u32int)(r3pcb2->stackTop);
		cp2->eip = (u32int) proc2;
		cp2->eflags = 0x202;

		insertPCB(r3pcb2);
	}

	if (!p3exists) {
		pcb *r3pcb3 = setupPCB(P3_NAME, 1, 1);
		r3pcb3->isSuspended = 1;
		context *cp3 = (context *)(r3pcb3->stackTop);
		memset(cp3, 0, sizeof(struct context));
		cp3->fs = 0x10;
		cp3->gs = 0x10;
		cp3->ds = 0x10;
		cp3->es = 0x10;
		cp3->cs = 0x8;
		cp3->ebp = (u32int)(r3pcb3->stackBottom);
		cp3->esp = (u32int)(r3pcb3->stackTop);
		cp3->eip = (u32int) proc3;
		cp3->eflags = 0x202;

		insertPCB(r3pcb3);
	}

	if (!p4exists) {
		pcb *r3pcb4 = setupPCB(P4_NAME, 1, 1);
		r3pcb4->isSuspended = 1;
		context *cp4 = (context *)(r3pcb4->stackTop);
		memset(cp4, 0, sizeof(struct context));
		cp4->fs = 0x10;
		cp4->gs = 0x10;
		cp4->ds = 0x10;
		cp4->es = 0x10;
		cp4->cs = 0x8;
		cp4->ebp = (u32int)(r3pcb4->stackBottom);
		cp4->esp = (u32int)(r3pcb4->stackTop);
		cp4->eip = (u32int) proc4;
		cp4->eflags = 0x202;

		insertPCB(r3pcb4);
	}

	if (!p5exists) {
		pcb *r3pcb5 = setupPCB(P5_NAME, 1, 1);
		r3pcb5->isSuspended = 1;
		context *cp5 = (context *)(r3pcb5->stackTop);
		memset(cp5, 0, sizeof(struct context));
		cp5->fs = 0x10;
		cp5->gs = 0x10;
		cp5->ds = 0x10;
		cp5->es = 0x10;
		cp5->cs = 0x8;
		cp5->ebp = (u32int)(r3pcb5->stackBottom);
		cp5->esp = (u32int)(r3pcb5->stackTop);
		cp5->eip = (u32int) proc5;
		cp5->eflags = 0x202;

		insertPCB(r3pcb5);
	}

	return "Processes added to queue.";
}
