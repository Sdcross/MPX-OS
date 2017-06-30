#ifndef _SERIAL_H
#define _SERIAL_H

#define COM1 0x3f8
#define COM2 0x2f8
#define COM3 0x3e8
#define COM4 0x2e8

/**
 * Initializes devices for user interaction, logging, ...
 *
 * @param device The device to initialize
 * @return The error code
 */
int init_serial(int device);

/**
 * Writes a message to the active serial output device.
 * Appends a newline character.
 *
 * @param msg The message to write
 * @return The error code
 */
int serial_println(const char *msg);

/**
 * Writes a message to the active serial output device.
 *
 * @param msg The message to write
 * @return  The error code
 */
int serial_print(const char *msg);

/**
 * Sets serial_port_out to the given device address. All serial
 * output, such as that from serial_println, will be directed to
 * this device.
 *
 * @param device The device to set as output
 * @return The error code
 */
int set_serial_out(int device);

/**
 * Sets serial_port_in to the given device address. All serial input,
 * such as console input via a virutal machine, QEMU/Bochc/etc, will
 * be directed to the device.
 *
 * @param device The divce to set as input
 * @return The error code
 */
int set_serial_in(int device);

#endif
