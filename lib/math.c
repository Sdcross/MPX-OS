#include <limits.h>
#include <math.h>

/**
 * Converts a BCD encoded byte to a decimal encoded byte
 *
 * @param bcd The value to convert
 * @return The decimal value
 */
unsigned char bcdToDec(unsigned char bcd) {
	return (((bcd >> 4) * 10) + (bcd & 0x0F));
}

/**
 * Converts a decimal encoded byte to a BCD encoded byte
 *
 * @param dec The value to convert
 * @return The BCD value
 */
unsigned char decToBcd(unsigned char dec) {
	if (dec > 99) {
		return UCHAR_MAX;
	}

	return (((dec / 10) << 4) + (dec % 10));
}
