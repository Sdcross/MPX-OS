#ifndef _TIME_H
#define _TIME_H

/**
 * Day Names
 */
#define MON ((const char*) "Monday")
#define TUE ((const char*) "Tuesday")
#define WED ((const char*) "Wednesday")
#define THU ((const char*) "Thursday")
#define FRI ((const char*) "Friday")
#define SAT ((const char*) "Saturday")
#define SUN ((const char*) "Sunday")

/**
 * Month Names
 */
#define JAN ((const char*) "January")
#define FEB ((const char*) "February")
#define MAR ((const char*) "March")
#define APR ((const char*) "April")
#define MAY ((const char*) "May")
#define JUN ((const char*) "June")
#define JUL ((const char*) "July")
#define AUG ((const char*) "August")
#define SEP ((const char*) "September")
#define OCT ((const char*) "October")
#define NOV ((const char*) "November")
#define DEC ((const char*) "December")

/**
 * Aliases for accessing time/date
 */
#define SECONDS     0x00
#define MINUTES     0x02
#define HOURS       0x04
#define DAY_WEEK    0x06
#define DAY_MONTH   0x07
#define MONTH       0x08
#define YEAR        0x09

/**
 * Registers for reading/writing data
 */
#define CONTROL_PORT    0x70
#define DATA_PORT       0x71

/**
 * The delimiter for the time
 */
#define TIME_DELIM  ':'

/**
 * NMI Flags
 * Disable - OR, Enable - AND
 */
#define NMI_DISABLE     0x80
#define NMI_ENABLE      0x7F

/**
 * Structure representing a date and time.
 */
typedef struct {
	int sec;
	int min;
	int hour;
	int day_w;
	int day_m;
	int day_y;
	int mon;
	int year;
} date_time;

extern const int DAYS_IN_MONTH[13];		// We use 13 here so that 0 can have 0 days - avoids having to shift months

// Get Time/Date functions
/**
 * Gets the date and time from the RTC registers.
 *
 * @return The date and time stored in the RTC.
 */
date_time getDateTime();

/**
 * Sets the date and time to the specified values.
 *
 * Day of month must be specified but day of week/year will be
 * automatically calculated.
 * @param dateTime The values to set.
 */
void setDateTime(date_time);

/**
 * Gets the seconds value (decimal-encoded) from the RTC.
 *
 * @return The decimal-encoded number of seconds.
 */
unsigned char getSeconds();

/**
 * Gets the minutes value (decimal-encoded) from the RTC.
 *
 * @return The decimal-encoded number of minutes.
 */
unsigned char getMinutes();

/**
 * Gets the hours value (decimal-encoded) from the RTC.
 *
 * @return The decimal-encoded number of hours.
 */
unsigned char getHours();

/**
 * Gets the day of the week (decimal-encoded) from the RTC.
 *
 * Sunday - 1
 * Monday - 2
 * Tuesday - 3
 * Wednesday - 4
 * Thursday - 5
 * Friday - 6
 * Saturday - 7
 *
 * @return The decimal-encoded day of the week.
 */
unsigned char getDayOfWeek();

/**
 * Gets the day of the month (decimal-encoded) from the RTC.
 *
 * @return The decimal-encoded day of the month.
 */
unsigned char getDayOfMonth();

/**
 * Gets the month (decimal-encoded) from the RTC.
 *
 * @return The decimal-encoded month.
 */
unsigned char getMonth();

/**
 * Gets the year (decimal-encoded) from the RTC.
 *
 * @return The decimal-encoded year.
 */
unsigned char getYear();


// Set Time/Date functions
/**
 * Sets the seconds register in the RTC.
 * This number should be decimal-encoded.
 *
 * @param seconds The seconds value to set
 */
void setSeconds(unsigned char seconds);

/**
 * Sets the minutes register in the RTC.
 * This number should be decimal-encoded.
 *
 * @param minutes The minutes value to set
 */
void setMinutes(unsigned char minutes);

/**
 * Sets the hours register in the RTC.
 * This number should be decimal-encoded.
 *
 * @param hours The hours value to set
 */
void setHours(unsigned char hours);

/**
 * Sets the day of the week register in the RTC.
 * This number should be decimal-encoded.
 *
 * Sunday - 1
 * Monday - 2
 * Tuesday - 3
 * Wednesday - 4
 * Thursday - 5
 * Friday - 6
 * Saturday - 7
 *
 * @param dayOfWeek The day of the week value to set
 */
void setDayOfWeek(unsigned char dayOfWeek);

/**
 * Sets the day of the month register in the RTC.
 * This number should be decimal-encoded.
 *
 * @param dayOfMonth The day of the month value to set
 */
void setDayOfMonth(unsigned char dayOfMonth);

/**
 * Sets the month register in the RTC.
 * This number should be decimal-encoded.
 *
 * @param month The month value to set
 */
void setMonth(unsigned char month);

/**
 * Sets the year register in the RTC.
 * This number should be decimal-encoded.
 *
 * @param year The year value to set
 */
void setYear(unsigned char year);


// Sets the day of the week/year in the date_time struct
/**
 * Sets the day of week property of the date_time struct
 * based on the year, month, and day of month values.
 *
 * Sunday - 1
 * Monday - 2
 * Tuesday - 3
 * Wednesday - 4
 * Thursday - 5
 * Friday - 6
 * Saturday - 7
 *
 * @param dateTime The date_time to update
 */
void updateDayOfWeek(date_time *dateTime);

/**
 * Sets the day of year property of the date_time struct
 * based on the year, month, and day of month values.
 *
 * @param dateTime The date_time to update
 */
void updateDayOfYear(date_time *dateTime);

/**
 * Determines if the given year is a leap year.
 *
 * @param year The year to check
 * @return True if the year is a leap year.
 */
int isLeapYear(int year);

#endif
