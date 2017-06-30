/*
  ----- regex.h -----

  Description..: Contains methods and variables used regex
*/

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
int testRegex(const char *regex, const char *stringToCheck);
