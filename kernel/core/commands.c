#include <core/commands.h>
#include <core/version.h>
#include <core/help.h>
#include <core/serial.h>

#include <time.h>
#include <string.h>
#include <regex.h>

#include <modules/mpx_supt.h>

/**
 * Returns the current version of the OS.
 *
 * Usage: version
 *
 * Args:
 * 	[no args] - Returns the version
 *
 * @param args The arguments to pass to the function
 * @return The version of the OS.
 */
const char *version(char **args, int numArgs) {
	// Hack to make compiler think the arguments are used
	numArgs = numArgs;
	args = args;

	return OS_VERSION;
}

/**
 * Returns the current date/time in ISO-8601 format.
 * Improperly specified date/times are ignored.
 *
 * Usage: date [--date] [--time] [--setdate yyyy-MM-dd] [--settime hh:mm:ss]
 *
 * Args:
 * 	[no args] - Return the date and time
 * 	--date - Return the date
 * 	--time - Return the time
 *	--setdate - Sets the date to the specified date (returns the new date)
 *	--settime - Sets the time to the specified time (returns the new time)
 *
 * @param args The arguments to pass to the function
 * @return The ISO-8601 formatted date
 */
const char *date(char **args, int numArgs) {
	char dateFlag = 0;
	char timeFlag = 0;
	char setDateFlag = 0;
	char setTimeFlag = 0;
	char newDate[20];
	char newTime[20];

	// Parse all of the passed in args and set appropriate flags
	int i;
	for (i = 0; i < numArgs; i++) {
		if (strcmp(args[i], "--date") == 0) {
			dateFlag = 1;
		} else if (strcmp(args[i], "--time") == 0) {
			timeFlag = 1;
		} else if (strcmp(args[i], "--setdate") == 0) {
			dateFlag = 1;

			if (i + 1 < numArgs && testRegex("dddd/-dd/-dd", args[++i])) {	// Validate input format
				setDateFlag = 1;
				strcpy(newDate, args[i]);
			}
		} else if (strcmp(args[i], "--settime") == 0) {
			timeFlag = 1;

			if (i + 1 < numArgs && testRegex("dd/:dd/:dd", args[++i])) {	// Validate input format
				setTimeFlag = 1;
				strcpy(newTime, args[i]);
			}
		}
	}

	// Append both the time and date if there are no args.
	if (numArgs == 0) {
		dateFlag = 1;
		timeFlag = 1;
	}

	// Get the current date/time
	date_time dateTime = getDateTime();

	// Set the new date
	if (setDateFlag) {
		int year = atoi(strtok(newDate, "-")) % 100; //year must be <100 because it is expected to be a 2 digit number, from 1970(70) - 2069(69)
		int month = atoi(strtok(NULL, "-"));
		int day = atoi(strtok(NULL, "-"));

		if (((0 <= year) && (year <= 99)) &&												// Validate the year
			((1 <= month) && (month <= 12)) &&												// Validate the month
			((1 <= day) && ((day <= DAYS_IN_MONTH[month]) ||								// Validate the day
					(month == 2 && isLeapYear(year) && day <= (DAYS_IN_MONTH[month] + 1))	// Validate the day on leap year in February
		))) {
			dateTime.year = year;
			dateTime.mon = month;
			dateTime.day_m = day;
		}
	}

	// Set the new time
	if (setTimeFlag) {
		int hour = atoi(strtok(newTime, ":"));
		int minute = atoi(strtok(NULL, ":"));
		int second = atoi(strtok(NULL, ":"));

		if (((0 <= hour) && (hour <= 23)) &&			// Validate the hour
			((0 <= minute) && (minute <= 59)) &&		// Validate the minute
			((0 <= second) && (second <= 59)))			// Validate the second
		{
			dateTime.hour = hour;
			dateTime.min = minute;
			dateTime.sec = second;
		}
	}

	// Update the date/time
	if (setDateFlag || setTimeFlag) {
		setDateTime(dateTime);
	}

	// Have to allocate memory since I'm returning this string.
	char *dateTimeStr = sys_alloc_mem(21);
	char numHolder[3];
	dateTimeStr[0] = '\0';

	// If date needs to be returned, stringify the date
	if (dateFlag) {
		if (dateTime.year >= 70) {
			strcat(dateTimeStr, "19");
		} else {
			strcat(dateTimeStr, "20");
		}

		itoa(dateTime.year, numHolder, 10);
		if (dateTime.year < 10) {
			strcat(dateTimeStr, "0");
		}
		strcat(dateTimeStr, numHolder);
		strcat(dateTimeStr, "-");
		itoa(dateTime.mon, numHolder, 10);
		if (dateTime.mon < 10) {
			strcat(dateTimeStr, "0");
		}
		strcat(dateTimeStr, numHolder);
		strcat(dateTimeStr, "-");
		itoa(dateTime.day_m, numHolder, 10);
		if (dateTime.day_m < 10) {
			strcat(dateTimeStr, "0");
		}
		strcat(dateTimeStr, numHolder);
	}

	// If returning both date and time, put the 'T' in between
	if (dateFlag && timeFlag) {
		strcat(dateTimeStr, "T");
	}

	// If date needs to be returned, stringify the time
	if (timeFlag) {
		itoa(dateTime.hour, numHolder, 10);
		if (dateTime.hour < 10) {
			strcat(dateTimeStr, "0");
		}
		strcat(dateTimeStr, numHolder);
		strcat(dateTimeStr, ":");
		itoa(dateTime.min, numHolder, 10);
		if (dateTime.min < 10) {
			strcat(dateTimeStr, "0");
		}
		strcat(dateTimeStr, numHolder);
		strcat(dateTimeStr, ":");
		itoa(dateTime.sec, numHolder, 10);
		if (dateTime.sec < 10) {
			strcat(dateTimeStr, "0");
		}
		strcat(dateTimeStr, numHolder);
		// strcat(dateTimeStr, "Z"); // Timezone, 'Z' = UTC but isn't required to be there
	}

	serial_println(dateTimeStr);
	sys_free_mem(dateTimeStr);

	return "";
}
