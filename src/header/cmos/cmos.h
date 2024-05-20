#ifndef _CMOS_H
#define _CMOS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "header/stdlib/string.h"

// Define the CMOS ports
#define CMOS_ADDRESS_PORT 0x70
#define CMOS_DATA_PORT 0x71

struct clock
{
    uint8_t front_second;
    uint8_t behind_second;
    uint8_t front_minute;
    uint8_t behind_minute;
    uint8_t front_hour;
    uint8_t behind_hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} __attribute__((packed));


struct time
{
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} __attribute__((packed));

/**
 * cmos_read_rtc - Read the current date and time from the CMOS RTC
 *
 * @param year      Output parameter for the current year (in BCD format)
 * @param month     Output parameter for the current month (in BCD format)
 * @param day       Output parameter for the current day of the month (in BCD format)
 * @param hour      Output parameter for the current hour (in BCD format)
 * @param minute    Output parameter for the current minute (in BCD format)
 * @param second    Output parameter for the current second (in BCD format)
 */
void cmos_read_rtc(struct time *t);

void cmos_read_clock(struct clock *t);

#endif