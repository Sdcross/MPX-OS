#ifndef _MATH_H
#define _MATH_H

/**
 * Converts a BCD encoded byte to a decimal encoded byte
 *
 * @param bcd The value to convert
 * @return The decimal value
 */
unsigned char bcdToDec(unsigned char bcd);

/**
 * Converts a decimal encoded byte to a BCD encoded byte
 *
 * @param dec The value to convert
 * @return The BCD value
 */
unsigned char decToBcd(unsigned char dec);

#endif
