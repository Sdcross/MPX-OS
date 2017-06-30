/*
  ----- kmain.c -----

  Description..: Kernel main. The first function called after
	  the bootloader. Initialization of hardware, system
	  structures, devices, and initial processes happens here.
*/

#include <stdint.h>
#include <string.h>
#include <system.h>

#include <core/io.h>
#include <core/serial.h>
#include <core/tables.h>
#include <core/interrupts.h>
#include <core/queue.h>
#include <core/comHandler.h>
#include <mem/heap.h>
#include <mem/paging.h>
#include <mem/memoryControl.h>
#include <modules/mpx_supt.h>

void kmain(void) {
	extern uint32_t magic;
	// Uncomment if you want to access the multiboot header
	// extern void *mbd;
	// char *boot_loader_name = (char*)((long*)mbd)[16];

	// Set up the heap.
	initializeHeap(500000);
	sys_set_malloc(allocateMemory);
	sys_set_free(deallocateMemory);

	// 0) Initialize Serial I/O and call mpx_init
	init_serial(COM1);
	set_serial_in(COM1);
	set_serial_out(COM1);
	mpx_init(MODULE_R5);

	klogv("Starting MPX boot sequence...");
	klogv("Initialized serial I/O on COM1 device...");

	// 1) Check that the boot was successful and correct when using grub
	// Comment this when booting the kernel directly using QEMU, etc.
	if (magic != 0x2BADB002) {
		//kpanic("Boot was not error free. Halting.");
	}

	// 2) Descriptor Tables
	klogv("Initializing descriptor tables...");
	init_idt();      // Initialize the interrupt descriptor table
	init_gdt();      // Initialize the global descriptor table
	init_irq();      // Initialize the interrupt handlers
	sti();           // Enable interrupts

	// 4) Virtual Memory
	klogv("Initializing virtual memory...");
	init_paging();

	// 5) Call Commhand
	klogv("Transferring control to commhand...");

	// Create the process for the command handler
	pcb *commHand = setupPCB("CommandHandler", 1, 1);
	context *commHandCp = (context *)(commHand->stackTop);
	memset(commHandCp, 0, sizeof(struct context));
	commHandCp->fs = 0x10;
	commHandCp->gs = 0x10;
	commHandCp->ds = 0x10;
	commHandCp->es = 0x10;
	commHandCp->cs = 0x8;
	commHandCp->ebp = (u32int)(commHand->stackBottom);
	commHandCp->esp = (u32int)(commHand->stackTop);
	commHandCp->eip = (u32int) initCommandHandler;
	commHandCp->eflags = 0x202;

	// Create the process for the idle process
	pcb *idleProc = setupPCB("Idle", 0, 0);
	context *idleCp = (context *)(idleProc->stackTop);
	memset(idleCp, 0, sizeof(struct context));
	idleCp->fs = 0x10;
	idleCp->gs = 0x10;
	idleCp->ds = 0x10;
	idleCp->es = 0x10;
	idleCp->cs = 0x8;
	idleCp->ebp = (u32int)(idleProc->stackBottom);
	idleCp->esp = (u32int)(idleProc->stackTop);
	idleCp->eip = (u32int) idle;
	idleCp->eflags = 0x202;

	// Inserts the processes into the queue
	insertPCB(commHand);
	insertPCB(idleProc);

	// Triggers software interrupt to start process dispatching
	asm volatile("int $60");

	// 11) System Shutdown
	klogv("Starting system shutdown procedure...");

	/* Shutdown Procedure */
	klogv("Shutdown complete. You may now turn off the machine. (QEMU: CTRL+a then x)");
	hlt();
}
