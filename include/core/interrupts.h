#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

/**
 * Installs the initial interrupt handlers for the first 32
 * irq lines. Most do a panic for now.
 */
void init_irq(void);

/**
 * Initializes the programmable interrupt controllers and performs
 * the necessary remapping of IRQs. Leaves interrupts turned off.
 */
void init_pic(void);

#endif
