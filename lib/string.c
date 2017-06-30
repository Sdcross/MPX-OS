#include <system.h>
#include <string.h>
#include <limits.h>

/* ****************************** *
 * ********* Your Task: ********* *
 * ******** complete the ******** *
 * * following string functions * *
 * ****************************** */

/**
 * Returns the length of a string.
 *
 * @param s The input string
 * @return The length of the string
 */
int strlen(const char *s) {
	int count = 0;

	while (*s != '\0') {
		count++;
		s++;
	}

	return count;
}

/**
 * Copy on string to another.
 *
 * @param cpy The destination string
 * @param ori The source string
 * @return A pointer to the destination string
 */
char *strcpy(char *cpy, const char *ori) {
	char *holder = cpy;
	while ((*cpy++ = *ori++));
	return holder;
}

/**
 * Determine if a character is a digit.
 *
 * @param c The character to check
 * @return True if the character is a digit
 */
int isdigit(const char c) {
	return (c >= '0') && (c <= '9');
}

/**
 * Convert an ASCII string to an integer
 *
 * @param s The string to convert
 * @return The integer value of the string, or the MAX/MIN value of an
 * 	integer if the value is out of range.
 */
int atoi(const char *s) {
	int ret = 0;		// The default return value
	int sign = 1;		// Positive number by default
	int overflow = 0;	// Overflow flag

	// Remove leading whitespace
	while (*s == ' ') {
		s++;
	}

	// Check if the number is designated negative, positive, or neither
	if (*s == '-') {
		sign = -1;	// It's a negative number, so set the sign to be -1
		s++;        // 1st character was a negative, no need to check this again
	} else if (*s == '+') {
		sign = 1;	// It's a positive number, so set the sign to be 1
		s++;        // 1st character was a positive, no need to check this again
	} else {
		sign = 1;	// Unnecessary, but for completeness
	}

	// Loop through the string until either a null terminator is found
	// or a non-numeric number is found.
	while (*s != '\0' && isdigit(*s)) {
		// There is another number that should become the ones place.
		// Shift everything left (multiply by ten) then add the value of 
		// the number on. This is the difference of the ASCII code values 
		// between that character and the '0' character
		int oldValue = ret;
		ret = (ret * 10) + (*s - '0');
		s++;

		// Check for overflow
		overflow = ret < oldValue;
	}

	// Return MIN/MAX if an overflow occurs
	if (overflow) {
		if (sign > 0) {
			return INT_MAX;
		} else {
			return INT_MIN;
		}
	}

	// Return the result with the correct sign.
	return sign * ret;
}

/**
 * Converts an integer to an ASCII string.
 *
 * @param num The number to convert
 * @param str The destination string
 * @param base The radix
 */
void itoa(int num, char *str, int base) {
	int isNegative = 0;
	int i = 0;

	// Handle 0 explicitely, otherwise empty string is printed for 0
	if (num == 0) {
		str[i++] = '0';
		str[i] = '\0';
		return;
	}

	// In standard itoa(), negative numbers are handled only with 
	// base 10. Otherwise numbers are considered unsigned.
	if (num < 0 && base == 10) {
		isNegative = 1;
		num = -num;
	}

	// Process individual digits
	while (num != 0) {
		int rem = num % base;
		str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		num = num / base;
	}

	// If number is negative, append '-'
	if (isNegative) {
		str[i++] = '-';
	}

	str[i] = '\0'; // Append string terminator

	// Reverse the string
	reverse(str, i);
}

/**
 * Reverses a string.
 *
 * @param str The string to reverse
 * @param len The length of the string
 */
void reverse(char *str, int len) {
	int start = 0;
	int end = len - 1;

	while (start < end) {
		char tmp = str[start];
		str[start] = str[end];
		str[end] = tmp;
		start++;
		end--;
	}
}

/**
 * Compares two strings to each other
 *
 * @param s1 The first string
 * @param s2 The second string
 * @return The difference between the characters at the first index of indifference
 */
int strcmp(const char *s1, const char *s2) {
	// Loop through all characters checking for eqaulity and the terminator
	while (*s1 == *s2 && *s1 != '\0') {
		s1++;
		s2++;
	}

	// Remarks:
	// 1) If we made it to the end of both strings (i. e. our pointer points to a
	//	'\0' character), the function will return 0
	// 2) If we didn't make it to the end of both strings, the function will
	//	return the difference of the characters at the first index of
	//	indifference.
	return (*(unsigned char *) s1 - *(unsigned char *) s2);
}

/* And finally....
   For the brave ones! (Note: you'll need to add a prototype to string.h)
   sprintf must work properly for the following types to receive extra credit:
	 1) characters
	 2) strings
	 3) signed integers
	 4) hexadecimal numbers may be useful
	 ...
	 \infty) Or feel free to completely implement sprintf
			 (Read the man Page: $ man sprintf)
   int sprintf(char *str, const char *format, ...); 
*/


/* ---------------------------------------
	Functions below this point are given.
	No need to tamper with these!
   --------------------------------------- */

/**
 * Concatenate the contents of one string onto another.
 *
 * @param s1 The destination string
 * @param s2 The source string
 * @return A pointer to the destination string
 */
char *strcat(char *s1, const char *s2) {
	char *rc = s1;
	if (*s1) while (*++s1);
	while ((*s1++ = *s2++));
	return rc;
}

/**
 * Determine if a character is whitespace.
 *
 * @param c The character to check
 * @return True if the character is a whitespace character
 */
int isspace(const char *c) {
	if (*c == ' ' ||
		*c == '\n' ||
		*c == '\r' ||
		*c == '\f' ||
		*c == '\t' ||
		*c == '\v') {
		return 1;
	}
	return 0;
}

/**
 * Checks if the given char is a-z or A-Z
 * @param const char c
 * @return 1 if c is a char, 0 otherwise
 */
int isChar(const char c){
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

/**
 * Checks if the given char is A-Z
 * @param const char c
 * @return 1 if c is a upper char, 0 otherwise
 */
int isUpperChar(const char c){
	return (c >= 'A' && c <= 'Z');
}

/**
 * Checks if the given char is a-z
 * @param const char c
 * @return 1 if c is a lower char, 0 otherwise
 */
int isLowerChar(const char c){
	return (c >= 'a' && c<= 'z');
}

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
char *strtok(char *s1, const char *s2) {
	static char *tok_tmp = NULL;
	const char *p = s2;

	//new string
	if (s1 != NULL) {
		tok_tmp = s1;
	}
		//old string cont'd
	else {
		if (tok_tmp == NULL) {
			return NULL;
		}
		s1 = tok_tmp;
	}

	//skip leading s2 characters
	while (*p && *s1) {
		if (*s1 == *p) {
			++s1;
			p = s2;
			continue;
		}
		++p;
	}

	//no more to parse
	if (!*s1) {
		return (tok_tmp = NULL);
	}

	//skip non-s2 characters
	tok_tmp = s1;
	while (*tok_tmp) {
		p = s2;
		while (*p) {
			if (*tok_tmp == *p++) {
				*tok_tmp++ = '\0';
				return s1;
			}
		}
		++tok_tmp;
	}

	//end of string
	tok_tmp = NULL;
	return s1;
}
