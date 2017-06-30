#ifndef _SYSTEM_H
#define _SYSTEM_H

#define NULL 0

/**
 * Suppress 'unused parameter' warnings/errors
 *
 * @param p The parameter
 */
#define no_warn(p) if (p) while (1) break

// Allows compilation with gcc -std=c89
// May also help avoid naming conflicts
#define asm __asm__
#define volatile __volatile__

/**
 * Turn IRQs on.
 */
#define sti()  asm volatile ("sti"::)

/**
 * Turn IRQs off.
 */
#define cli()  asm volatile ("cli"::)

/**
 * Skip cycle.
 */
#define nop()  asm volatile ("nop"::)

/**
 * Halt.
 */
#define hlt()  asm volatile ("hlt"::)

/**
 * Interrupt return.
 */
#define iret() asm volatile ("iret"::)

/**
 * Kernel code segment ID.
 */
#define GDT_CS_ID 0x01

/**
 * Kernel data segment ID.
 */
#define GDT_DS_ID 0x02

/* System Types */
typedef unsigned int size_t;
typedef unsigned char u8int;
typedef unsigned short u16int;
typedef unsigned long u32int;

/**
 * Tests if interrupts are on.
 *
 * @return True if interrupts are on
 */
static inline int irq_on() {
	int f;
	asm volatile ("pushf\n\t"
			"popl %0"
	: "=g"(f));
	return f & (1 << 9);
}

/**
 * Kernel log message. Sent to active serial device.
 *
 * @param msg The message to log
 */
void klogv(const char *msg);

/**
 * Kernel panic. Prints an error message and halts.
 *
 * @param msg The error mesage to print
 */
void kpanic(const char *msg);

#endif
