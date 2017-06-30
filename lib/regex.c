/*
  ----- regex.h -----

  Description..: Contains methods and variables used regex
*/

#include <stdint.h>
#include <string.h>
#include <regex.h>

/**
 * Tests if the stringToCheck adheres to the given regex string
 *
 * The regex string is comprised of:
 * d matches digits 0-9
 * c matches characters a-zA-Z
 * u matches uppercase character, A-Z
 * l matches lowercase character, a-z
 * * matches any char
 * /char for a literal character, ex: "/a" matches 'a', /d matches 'd'
 *
 * Example : regex "dcd/a" matches any string with the pattern "digit character digit 'a'", ex "1b3a", "6b9a"
 *
 * @param regex
 * @param stringToCheck
 * @return 1 if adheres, 0 otherwise
 */
int testRegex(const char *regex, const char *stringToCheck) {
	int nextIsLiteral = 0;
	int i = 0;
	int j = 0;
	for (; i < strlen(regex); i += 1) {
		if (nextIsLiteral == 1) {
			if (regex[i] != stringToCheck[j]) {
				return 0;
			}
			j += 1;
			nextIsLiteral = 0;
		}
		else if (regex[i] == '/') {
			nextIsLiteral = 1;
			continue;
		}
		else if (regex[i] == 'd') { //digit
			if (isdigit(stringToCheck[j]) != 1) {
				return 0;
			}
			j += 1;
		}
		else if (regex[i] == 'c') { //character
			if (isChar(stringToCheck[j]) != 1) {
				return 0;
			}
			j += 1;
		}
		else if (regex[i] == 'u') { //uppercase character
			if (isUpperChar(stringToCheck[j]) != 1) {
				return 0;
			}
			j += 1;
		}
		else if (regex[i] == 'l') { //lowercase character
			if (isLowerChar(stringToCheck[j]) != 1) {
				return 0;
			}
			j += 1;
		}
		else if (regex[i] == '*') { //any character
			if (stringToCheck[j] == '\0') {
				return 0;
			}
			j += 1;
		}
		else {
			return 0;
		}
	}
	if (strlen(stringToCheck) != j) {
		return 0;
	}
	return 1;
}