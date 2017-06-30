#ifndef _IO_H
#define _IO_H

/**
 * Writes a byte of data to a port.
 *
 * @param port The port to write the data to
 * @param data The byte to write
 */
#define outb(port, data)                                    \
  asm volatile ("outb %%al,%%dx" : : "a" (data), "d" (port))

/**
 * Reads a byte of data from a port.
 *
 * @param port The port to read the data from
 * @return The byte from the port
 */
#define inb(port) ({                                        \
      unsigned char r;                                      \
      asm volatile ("inb %%dx,%%al": "=a" (r): "d" (port)); \
      r;                                                    \
    })

#endif
