#include <string.h>
#include <system.h>

#include <core/serial.h>

/**
 * Kernel log message. Sent to active serial device.
 *
 * @param msg The message to log
 */
void klogv(const char *msg) {
	char logmsg[512] = {'\0'}, prefix[] = "klogv: ";
	strcat(logmsg, prefix);
	strcat(logmsg, msg);
	serial_println(logmsg);
}

/**
 * Kernel panic. Prints an error message and halts.
 *
 * @param msg The error mesage to print
 */
void kpanic(const char *msg) {
	cli(); //disable interrupts
	char logmsg[512] = {'\0'}, prefix[] = "\nPanic: ";
	strcat(logmsg, prefix);
	strcat(logmsg, msg);
	klogv(logmsg);
	hlt(); //halt
}
