#include <modules/mpx_supt.h>
#include <mem/heap.h>
#include <core/queue.h>
#include <core/pcb.h>

param params;
int current_module = -1;
void *(*student_malloc)(int);

pcb* cop = NULL;
context* callerContext;

boolean (*student_free)(void *);

/**
 * Changes the currently running process to that of the next ready process
 *
 * @param registers - copy of register values
 * @return u32int position of stackTop
 */
u32int* sys_call(context *registers){
	if(cop == NULL){
		callerContext = registers;
	}
	else {
		if(params.op_code == IDLE){
			cop->stackTop = (unsigned char*)registers;
			insertPCB(cop);
		}
		if(params.op_code == EXIT){
			removePCB(cop);
			freePCB(cop); //doesnt work yet
		}
	}

	if(getReadyQueue() != NULL){
		cop = popReady();
		return (u32int*)cop->stackTop;
	}
	cop = NULL;

	return (u32int*)callerContext;
}


/**
 * Set a region of memory
 *
 * @param s Destination
 * @param c Byte to write
 * @param n Count
 * @return s
 */
void *memset(void *s, int c, size_t n) {
	unsigned char *p = (unsigned char *) s;
	while (n--) {
		*p++ = (unsigned char) c;
	}
	return s;
}

/**
 * Generates interrupt 60H
 *
 * @param op_code (IDLE)
 * @return 0
 */
int sys_req(int op_code) {
	params.op_code = op_code;
	asm volatile ("int $60");
	return 0;
}

/**
 * Initialize MPX support software
 *
 * @param cur_mod (symbolic constants MODULE_R1, MODULE_R2, etc)
 */
void mpx_init(int cur_mod) {
	current_module = cur_mod;
}

/**
 * Sets the memory allocation function for sys_alloc_mem
 *
 * @param func Function pointer to the memory allocator
 */
void sys_set_malloc(void *(*func)(int)) {
	student_malloc = func;
}

/**
 * Sets the memory free function for sys_free_mem
 *
 * @param func Function pointer to the memory free-er
 */
void sys_set_free(boolean (func)(void *)) {
	student_free = func;
}

/**
 * Allocates a block of memory (similar to malloc)
 *
 * @param size Number of bytes to allocate
 * @return The allocated memory
 */
void *sys_alloc_mem(u32int size) {
	if (current_module < MODULE_R5)
		return (void *) kmalloc(size);
	else
		return (void *) (*student_malloc)(size);
	//todo this
}

/**
 * Frees memory
 *
 * @param ptr Pointer to the block of memory to free
 * @return
 */
int sys_free_mem(void *ptr) {
	if (current_module >= MODULE_R5)
		//todo do this
		return (*student_free)(ptr);
	// otherwise we don't free anything
	return -1;
}

/**
 * The idle process
 */
void idle() {
	while (1) {
		sys_req(IDLE);
	}
}

/**
 * Gets the name of the COP
 *
 * @return const char pointer name
 */
const char * getCOPName(){
	if(cop == NULL){
		return "BOOTSTRAPPER";
	}
	return cop->processName;

}
