/*
  ----- paging.c -----

  Description..: Routines used in initializing and
	maintaining page directories, page tables, pages
	and frames.
*/

#include <system.h>
#include <modules/mpx_supt.h>

#include "mem/heap.h"
#include "mem/paging.h"

u32int mem_size = 0x4000000; //64MB
u32int page_size = 0x1000; //4KB

u32int nframes; //number of frames
u32int *frames; //bitmap of frames

page_dir *kdir = 0; //kernel directory
page_dir *cdir = 0; //current directory

//defined in heap.c
extern u32int phys_alloc_addr;
extern heap *kheap;

/**
 * Marks a page frame bit as in use (1).
 *
 * @param addr The address of the frame
 */
void set_bit(u32int addr) {
	u32int frame = addr / page_size;
	u32int index = frame / 32;
	u32int offset = frame % 32;
	frames[index] |= (1 << offset);
}

/**
 * Marks a page frame bit as free (0).
 *
 * @param addr The address of the frame
 */
void clear_bit(u32int addr) {
	u32int frame = addr / page_size;
	u32int index = frame / 32;
	u32int offset = frame % 32;
	frames[index] &= ~(1 << offset);
}

/**
 * Checks if page frame is in use.
 *
 * @param addr The address of the frame
 * @return True if it is in use
 */
u32int get_bit(u32int addr) {
	u32int frame = addr / page_size;
	u32int index = frame / 32;
	u32int offset = frame % 32;
	return (frames[index] & (1 << offset));
}

/**
 * Finds the first free page frame.
 *
 * @return The first free page frame
 */
u32int first_free() {
	u32int i, j;
	for (i = 0; i < nframes / 32; i++)
		if (frames[i] != 0xFFFFFFFF) //if frame not full
			for (j = 0; j < 32; j++) //find first free bit
				if (!(frames[i] & (1 << j)))
					return i * 32 + j;

	return -1; //no free frames
}

/**
 * Finds and returns a page, allocating a new page table if necessary.
 *
 * @param addr The address of the page
 * @param dir The page directory
 * @param make_table Boolean to create a table if necessary
 * @return A pointer to the page
 */
page_entry *get_page(u32int addr, page_dir *dir, int make_table) {
	u32int phys_addr;
	u32int index = addr / page_size / 1024;
	u32int offset = addr / page_size % 1024;

	//return it if it exists
	if (dir->tables[index])
		return &dir->tables[index]->pages[offset];

		//create it
	else if (make_table) {
		dir->tables[index] = (page_table *) _kmalloc(sizeof(page_table), 1, &phys_addr);
		dir->tables_phys[index] = phys_addr | 0x7; //enable present, writable
		return &dir->tables[index]->pages[offset];
	} else return 0;
}

/**
 * Initializes the kernel page directory and initial kernel heap area. Performs
 * identity mapping of the kernel frames such that the virtual addresses are
 * equivalent to the physical addresses.
 */
void init_paging() {
	//create frame bitmap
	nframes = (u32int)(mem_size / page_size);
	frames = (u32int *) kmalloc(nframes / 32);
	memset(frames, 0, nframes / 32);

	//create kernel directory
	kdir = (page_dir *) _kmalloc(sizeof(page_dir), 1, 0); //page aligned
	memset(kdir, 0, sizeof(page_dir));

	//get pages for kernel heap
	u32int i = 0x0;
	for (i = KHEAP_BASE; i < (KHEAP_BASE + KHEAP_MIN); i += 1) {
		get_page(i, kdir, 1);
	}

	//perform identity mapping of used memory
	//note: placement_addr gets incremented in get_page,
	//so we're mapping the first frames as well
	i = 0x0;
	while (i < (phys_alloc_addr + 0x10000)) {
		new_frame(get_page(i, kdir, 1));
		i += page_size;
	}

	//allocate heap frames now that the placement addr has increased.
	//placement addr increases here for heap
	for (i = KHEAP_BASE; i < (KHEAP_BASE + KHEAP_MIN); i += PAGE_SIZE) {
		new_frame(get_page(i, kdir, 1));
	}

	//load the kernel page directory; enable paging
	load_page_dir(kdir);

	//setup the kernel heap
	kheap = make_heap(KHEAP_BASE, KHEAP_SIZE, KHEAP_BASE + KHEAP_MIN);
}

/**
 * Sets a page directory as the current directory and enables paging via the CR0
 * register, The CR3 register enables address translation from linear to physical
 * address.
 *
 * http://en.wikipedia.org/wiki/Control_register#Control_registers_in_x86_series
 *
 * @param new_page_dir The page directory to set as the current
 */
void load_page_dir(page_dir *new_dir) {
	cdir = new_dir;
	asm volatile ("mov %0,%%cr3"::"b"(&cdir->tables_phys[0]));
	u32int cr0;
	asm volatile ("mov %%cr0,%0": "=b"(cr0));
	cr0 |= 0x80000000;
	asm volatile ("mov %0,%%cr0"::"b"(cr0));
}

/**
  * Marks a frame as in use un the frame bitmap,
    sets up the page, and saves* the frame index in the page.
*
* @param page The page to create the frame in
 */void new_frame(page_entry *page) {

	u32int index;
	if (page->frameaddr != 0) return;
	if ((u32int)(-1) == (index = first_free())) kpanic("Out of memory");

	//mark a frame as in-use
	set_bit(index * page_size);
	page->present = 1;
	page->frameaddr = index;
	page->writeable = 1;
	page->usermode = 0;
}
