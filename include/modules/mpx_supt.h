#ifndef _MPX_SUPT_H
#define _MPX_SUPT_H

#include <core/queue.h>
#include <core/pcb.h>

#include <boolean.h>
#include <system.h>

#define EXIT 0
#define IDLE 1
#define READ 2
#define WRITE 3

#define MODULE_R1 0
#define MODULE_R2 1
#define MODULE_R3 2
#define MODULE_R4 4
#define MODULE_R5 8

typedef struct {
	int op_code;
	int device_id;
} param;

typedef struct context {
	u32int gs, fs, es, ds;
	u32int edi, esi, ebp, esp, ebx, edx, ecx, eax;
	u32int eip, cs, eflags;
} context;

u32int* sys_call(context *registers);


/**
 * Set a region of memory
 *
 * @param s Destination
 * @param c Byte to write
 * @param n Count
 * @return s
 */
void *memset(void *s, int c, size_t n);

/**
 * Generates interrupt 60H
 *
 * @param op_code (IDLE)
 * @return 0
 */
int sys_req(int op_code);

/**
 * Initialize MPX support software
 *
 * @param cur_mod (symbolic constants MODULE_R1, MODULE_R2, etc)
 */
void mpx_init(int cur_mod);

/**
 * Sets the memory allocation function for sys_alloc_mem
 *
 * @param func Function pointer to the memory allocator
 */
void sys_set_malloc(void *(*func)(int));

/**
 * Sets the memory free function for sys_free_mem
 *
 * @param func Function pointer to the memory free-er
 */
void sys_set_free(boolean (func)(void *));

/**
 * Allocates a block of memory (similar to malloc)
 *
 * @param size Number of bytes to allocate
 * @return The allocated memory
 */
void *sys_alloc_mem(u32int size);

/**
 * Frees memory
 *
 * @param ptr Pointer to the block of memory to free
 * @return
 */
int sys_free_mem(void *ptr);

/**
 * The idle process
 */
void idle();

/**
 * Gets the name of the COP
 *
 * @return const char pointer name
 */
const char * getCOPName();
#endif
