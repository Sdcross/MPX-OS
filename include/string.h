#ifndef _STRING_H
#define _STRING_H

#include <system.h>

/**
 * Determine if a character is whitespace.
 *
 * @param c The character to check
 * @return True if the character is a whitespace character
 */
int isspace(const char *c);

/**
 * Determine if a character is a digit.
 *
 * @param c The character to check
 * @return True if the character is a digit
 */
int isdigit(const char c);

/**
 * Checks if the given char is a-z or A-Z
 * @param const char c
 * @return 1 if c is a char, 0 otherwies
 */
int isChar(const char c);

/**
 * Checks if the given char is A-Z
 * @param const char c
 * @return 1 if c is a upper char, 0 otherwise
 */
int isUpperChar(const char c);

/**
 * Checks if the given char is a-z
 * @param const char c
 * @return 1 if c is a lower char, 0 otherwise
 */
int isLowerChar(const char c);


/**
 * Copy on string to another.
 *
 * @param cpy The destination string
 * @param ori The source string
 * @return A pointer to the destination string
 */
char *strcpy(char *s1, const char *s2);

/**
 * Concatenate the contents of one string onto another.
 *
 * @param s1 The destination string
 * @param s2 The source string
 * @return A pointer to the destination string
 */
char *strcat(char *s1, const char *s2);

/**
 * Returns the length of a string.
 *
 * @param s The input string
 * @return The length of the string
 */
int strlen(const char *s);

/**
 * Compares two strings to each other
 *
 * @param s1 The first string
 * @param s2 The second string
 * @return The difference between the characters at the first index of indifference
 */
int strcmp(const char *s1, const char *s2);

/**
 * Split string into tokens
 *
 * Call this function multiple times (substituting NULL for s1) until
 * NULL is returned to get all tokens.
 *
 * @param s1 The string to split
 * @param s2 The delimiter
 * @return A single token
 */
char *strtok(char *s1, const char *s2);

/**
 * Convert an ASCII string to an integer
 *
 * @param s The string to convert
 * @return The integer value of the string, or the MAX/MIN value of an
 * 	integer if the value is out of range.
 */
int atoi(const char *s);

/**
 * Converts an integer to an ASCII string.
 *
 * @param num The number to convert
 * @param str The destination string
 * @param base The radix
 */
void itoa(int num, char *str, int base);

/**
 * Reverses a string.
 *
 * @param str The string to reverse
 * @param len The length of the string
 */
void reverse(char *str, int len);

#endif
