/*
  ----- heap.c -----

  Description..: Heap management procedures.
*/

#include <system.h>
#include <string.h>

#include <core/serial.h>
#include <mem/heap.h>
#include <mem/paging.h>

heap *kheap = 0; //kernel heap
heap *curr_heap = 0; //current heap

extern page_dir *kdir; //kernel page directory
extern void *end, _end, __end; //kernel end; defined in link.ld

//physical placement address
//current physical memory allocation address
u32int phys_alloc_addr = (u32int) & end;

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
u32int _kmalloc(u32int size, int page_align, u32int *phys_addr) {
	u32int *addr;

	// Allocate on the kernel heap if one has been created
	if (kheap != 0) {
		addr = (u32int *) alloc(size, kheap, page_align);
		if (phys_addr) {
			page_entry *page = get_page((u32int) addr, kdir, 0);
			*phys_addr = (page->frameaddr * 0x1000) + ((u32int) addr & 0xFFF);
		}
		return (u32int) addr;
	}
		// Else, allocate directly from physical memory
	else {
		if (page_align && (phys_alloc_addr & 0xFFFFF000)) {
			phys_alloc_addr &= 0xFFFFF000;
			phys_alloc_addr += 0x1000;
		}
		addr = (u32int *) phys_alloc_addr;
		if (phys_addr) {
			*phys_addr = phys_alloc_addr;
		}
		phys_alloc_addr += size;
		return (u32int) addr;
	}
}

/**
 * Standard memory allocation routine. Use this unless you need to
 * specify alignment or obtain a physical address. Calls _kmalloc.
 *
 * @param size The amount of memory to allocate
 * @return The memory address
 */
u32int kmalloc(u32int size) {
	return _kmalloc(size, 0, 0);
}

/**
 * Allocates some memory using the given heap. Can specify page-alignment.
 *
 * @param size The amount of memory to allocate
 * @param hp The heap to allocate on
 * @param align The page alignment
 * @return The memory address
 */
u32int alloc(u32int size, heap *h, int align) {
	no_warn(size || align || h);
	static u32int heap_addr = KHEAP_BASE;

	u32int base = heap_addr;
	heap_addr += size;

	if (heap_addr > KHEAP_BASE + KHEAP_MIN)
		serial_println("Heap is full!");

	return base;
}

/**
 * Create a new heap.
 *
 * @param base Physical start address of the heap
 * @param max Maximum size the heap may grow to
 * @param min Minium/Initial size
 * @return The address of the heap
 */
heap *make_heap(u32int base, u32int max, u32int min) {
	no_warn(base || max || min);
	return (heap *) kmalloc(sizeof(heap));
}
