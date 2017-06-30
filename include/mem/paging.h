#ifndef _PAGING_H
#define _PAGING_H

#include <system.h>

#define PAGE_SIZE 0x1000

/**
 * Page entry structure
 * Describes a single page in memory
 */
typedef struct {
	u32int present   : 1;
	u32int writeable : 1;
	u32int usermode  : 1;
	u32int accessed  : 1;
	u32int dirty     : 1;
	u32int reserved  : 7;
	u32int frameaddr : 20;
} page_entry;


/**
 * Page table structure
 * Contains 1024 pages/frames
 */
typedef struct {
	page_entry pages[1024];
} page_table;

/**
 * Page directory structure
 * Limited to 1024 tables for now
 */
typedef struct {
	page_table *tables[1024];
	u32int tables_phys[1024];
} page_dir;

/**
 * Marks a page frame bit as in use (1).
 *
 * @param addr The address of the frame
 */
void set_bit(u32int addr);

/**
 * Marks a page frame bit as free (0).
 *
 * @param addr The address of the frame
 */
void clear_bit(u32int addr);

/**
 * Checks if page frame is in use.
 *
 * @param addr The address of the frame
 * @return True if it is in use
 */
u32int get_bit(u32int addr);

/**
 * Finds the first free page frame.
 *
 * @return The first free page frame
 */
u32int first_free();

/**
 * Initializes the kernel page directory and initial kernel heap area. Performs
 * identity mapping of the kernel frames such that the virtual addresses are
 * equivalent to the physical addresses.
 */
void init_paging();

/**
 * Sets a page directory as the current directory and enables paging via the CR0
 * register,The CR3 register enables address translation from linear to physical address.
 *
 * http://en.wikipedia.org/wiki/Control_register#Control_registers_in_x86_series
 * @param new_page_dir The page directory to set as the current
 */
void load_page_dir(page_dir *new_page_dir);

/**
 * Finds and returns a page, allocating a new page table if necessary.
 *
 * @param addr The address of the page
 * @param dir The page directory
 * @param make_table Boolean to create a table if necessary
 * @return A pointer to the page
 */
page_entry *get_page(u32int addr, page_dir *dir, int make_table);

/**
  * Marks a frame as in use un the frame bitmap,
    sets up the page, and saves* the frame index in the page.*
 * @param page The page to create the frame in
*/
void new_frame(page_entry *page);

#endif
