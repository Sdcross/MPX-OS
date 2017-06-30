#ifndef _HEAP_H
#define _HEAP_H

/**
 * Kernel heap.
 */
#define TABLE_SIZE 0x1000
#define KHEAP_BASE 0xD000000
#define KHEAP_MIN  0x10000
#define KHEAP_SIZE 0x1000000

/**
 * Heap allocation header.
 */
typedef struct {
	int size;
	int index_id;
} header;

/**
 * Heap allocation footer.
 */
typedef struct {
	header head;
} footer;

typedef struct {
	int size;
	int empty;
	u32int block;
} index_entry;

/**
 * Kernel heap index table.
 */
typedef struct {
	index_entry table[TABLE_SIZE];
	int id;
} index_table;

/**
 * Heap structure
 */
typedef struct {
	index_table index;
	u32int base;
	u32int max_size;
	u32int min_size;
} heap;

/**
 * Base-level kernel memory allocation routine. Used to provide page
 * alignment and access physical addresses of allocations.
 * Called by kmalloc with align=0, physical_address=0.
 *
 * @param size The amount of memory to allocate
 * @param align The page alignment
 * @param phys_addr The physical address
 * @return The memory address
 */
u32int _kmalloc(u32int size, int align, u32int *phys_addr);

/**
 * Standard memory allocation routine. Use this unless you need to
 * specify alignment or obtain a physical address. Calls _kmalloc.
 *
 * @param size The amount of memory to allocate
 * @return The memory address
 */
u32int kmalloc(u32int size);

/**
 * Free kernel memory.
 *
 * @return
 */
u32int kfree();

/**
 * Initialize the kernel heap, and set it as the current heap.
 */
void init_kheap();

/**
 * Allocates some memory using the given heap. Can specify page-alignment.
 *
 * @param size The amount of memory to allocate
 * @param hp The heap to allocate on
 * @param align The page alignment
 * @return The memory address
 */
u32int alloc(u32int size, heap *hp, int align);

/**
 * Create a new heap.
 *
 * @param base Physical start address of the heap
 * @param max Maximum size the heap may grow to
 * @param min Minium/Initial size
 * @return The address of the heap
 */
heap *make_heap(u32int base, u32int max, u32int min);

#endif
