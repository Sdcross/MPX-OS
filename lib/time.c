#include <core/io.h>
#include <system.h>
#include <time.h>
#include <math.h>

const int DAYS_IN_MONTH[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/**
 * Gets the date and time from the RTC registers.
 *
 * @return The date and time stored in the RTC.
 */
date_time getDateTime() {
	date_time dateTime;

	dateTime.sec = getSeconds();
	dateTime.min = getMinutes();
	dateTime.hour = getHours();
	dateTime.day_w = getDayOfWeek();
	dateTime.day_m = getDayOfMonth();
	dateTime.mon = getMonth();
	dateTime.year = getYear();
	updateDayOfYear(&dateTime);

	return dateTime;
}

/**
 * Sets the date and time to the specified values.
 *
 * Day of month must be specified but day of week/year will be
 * automatically calculated.
 * @param dateTime The values to set.
 */
void setDateTime(date_time dateTime) {
	updateDayOfWeek(&dateTime);
	updateDayOfYear(&dateTime);

	setSeconds(dateTime.sec);
	setMinutes(dateTime.min);
	setHours(dateTime.hour);
	setDayOfWeek(dateTime.day_w);
	setDayOfMonth(dateTime.day_m);
	setMonth(dateTime.mon);
	setYear(dateTime.year);
}

/**
 * Gets the seconds value (decimal-encoded) from the RTC.
 *
 * @return The decimal-encoded number of seconds.
 */
unsigned char getSeconds() {
	char seconds;

	outb(CONTROL_PORT, SECONDS);
	seconds = inb(DATA_PORT);

	return bcdToDec(seconds);
}

/**
 * Gets the minutes value (decimal-encoded) from the RTC.
 *
 * @return The decimal-encoded number of minutes.
 */
unsigned char getMinutes() {
	char minutes;

	outb(CONTROL_PORT, MINUTES);
	minutes = inb(DATA_PORT);

	return bcdToDec(minutes);
}

/**
 * Gets the hours value (decimal-encoded) from the RTC.
 *
 * @return The decimal-encoded number of hours.
 */
unsigned char getHours() {
	char hours;

	outb(CONTROL_PORT, HOURS);
	hours = inb(DATA_PORT);

	return bcdToDec(hours);
}

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
unsigned char getDayOfWeek() {
	char dayWeek;

	outb(CONTROL_PORT, DAY_WEEK);
	dayWeek = inb(DATA_PORT);

	return bcdToDec(dayWeek);
}

/**
 * Gets the day of the month (decimal-encoded) from the RTC.
 *
 * @return The decimal-encoded day of the month.
 */
unsigned char getDayOfMonth() {
	char dayMonth;

	outb(CONTROL_PORT, DAY_MONTH);
	dayMonth = inb(DATA_PORT);

	return bcdToDec(dayMonth);
}

/**
 * Gets the month (decimal-encoded) from the RTC.
 *
 * @return The decimal-encoded month.
 */
unsigned char getMonth() {
	char month;

	outb(CONTROL_PORT, MONTH);
	month = inb(DATA_PORT);

	return bcdToDec(month);
}

/**
 * Gets the year (decimal-encoded) from the RTC.
 *
 * @return The decimal-encoded year.
 */
unsigned char getYear() {
	char year;

	outb(CONTROL_PORT, YEAR);
	year = inb(DATA_PORT);

	return bcdToDec(year);
}

/**
 * Sets the seconds register in the RTC.
 * This number should be decimal-encoded.
 *
 * @param seconds The seconds value to set
 */
void setSeconds(unsigned char sec) {
	cli();
	outb(CONTROL_PORT, SECONDS | NMI_DISABLE);
	outb(DATA_PORT, decToBcd(sec));
	outb(CONTROL_PORT, SECONDS & NMI_ENABLE);
	sti();
}

/**
 * Sets the minutes register in the RTC.
 * This number should be decimal-encoded.
 *
 * @param minutes The minutes value to set
 */
void setMinutes(unsigned char min) {
	cli();
	outb(CONTROL_PORT, MINUTES | NMI_DISABLE);
	outb(DATA_PORT, decToBcd(min));
	outb(CONTROL_PORT, MINUTES & NMI_ENABLE);
	sti();
}

/**
 * Sets the hours register in the RTC.
 * This number should be decimal-encoded.
 *
 * @param hours The hours value to set
 */
void setHours(unsigned char hour) {
	cli();
	outb(CONTROL_PORT, HOURS | NMI_DISABLE);
	outb(DATA_PORT, decToBcd(hour));
	outb(CONTROL_PORT, HOURS & NMI_ENABLE);
	sti();
}

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
void setDayOfWeek(unsigned char day) {
	cli();
	outb(CONTROL_PORT, DAY_WEEK | NMI_DISABLE);
	outb(DATA_PORT, decToBcd(day));
	outb(CONTROL_PORT, DAY_WEEK & NMI_ENABLE);
	sti();
}

/**
 * Sets the day of the month register in the RTC.
 * This number should be decimal-encoded.
 *
 * @param dayOfMonth The day of the month value to set
 */
void setDayOfMonth(unsigned char day) {
	cli();
	outb(CONTROL_PORT, DAY_MONTH | NMI_DISABLE);
	outb(DATA_PORT, decToBcd(day));
	outb(CONTROL_PORT, DAY_MONTH & NMI_ENABLE);
	sti();
}

/**
 * Sets the month register in the RTC.
 * This number should be decimal-encoded.
 *
 * @param month The month value to set
 */
void setMonth(unsigned char mon) {
	cli();
	outb(CONTROL_PORT, MONTH | NMI_DISABLE);
	outb(DATA_PORT, decToBcd(mon));
	outb(CONTROL_PORT, MONTH & NMI_ENABLE);
	sti();
}

/**
 * Sets the year register in the RTC.
 * This number should be decimal-encoded.
 *
 * @param year The year value to set
 */
void setYear(unsigned char year) {
	cli();
	outb(CONTROL_PORT, YEAR | NMI_DISABLE);
	outb(DATA_PORT, decToBcd(year));
	outb(CONTROL_PORT, YEAR & NMI_ENABLE);
	sti();
}

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
 * @param dateTime The date_time to update.
 */
void updateDayOfWeek(date_time *dateTime) {
	// This is an implementation from Wikipedia
	static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	int d = dateTime->day_m;
	int m = dateTime->mon;
	int y = dateTime->year;

	y -= m < 3;
	dateTime->day_w = (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7 + 1;
}

/**
 * Sets the day of year property of the date_time struct
 * based on the year, month, and day of month values.
 *
 * @param dateTime The date_time to update.
 */
void updateDayOfYear(date_time *dateTime) {
	// This algorithm is from Barry Andersen on StackOverflow
	// http://stackoverflow.com/a/30169668
	int d = dateTime->day_m;
	int m = dateTime->mon;
	int y = dateTime->year;

	dateTime->day_y = ((275 * m) / 9) - (isLeapYear(y) ? 1 : 2) * ((m + 9) / 12) + d - 30;
}

/**
 * Determines if the given year is a leap year.
 *
 * @param year The year to check
 * @return True if the year is a leap year.
 */
int isLeapYear(int year) {
	return ((year & 3) == 0 && ((year % 25) != 0 || (year & 15) == 0));
}
