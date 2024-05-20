#include "cmos.h"
#include "../cpu/portio.h"

void cmos_read_rtc(struct time *t)
{
    // Read the CMOS time registers
    out(CMOS_ADDRESS_PORT, 0x00);
    t->second = in(CMOS_DATA_PORT);
    out(CMOS_ADDRESS_PORT, 0x02);
    t->minute = in(CMOS_DATA_PORT);
    out(CMOS_ADDRESS_PORT, 0x04);
    t->hour = in(CMOS_DATA_PORT);
    out(CMOS_ADDRESS_PORT, 0x07);
    t->day = in(CMOS_DATA_PORT);
    out(CMOS_ADDRESS_PORT, 0x08);
    t->month = in(CMOS_DATA_PORT);
    out(CMOS_ADDRESS_PORT, 0x09);
    t->year = in(CMOS_DATA_PORT);
    out(CMOS_ADDRESS_PORT, 0x32);
    t->year |= in(CMOS_DATA_PORT) << 8;

    // Convert BCD values to binary
    t->second = ((t->second & 0xF0) >> 4) * 10 + (t->second & 0x0F);
    t->minute = ((t->minute & 0xF0) >> 4) * 10 + (t->minute & 0x0F);

    t->hour = ((t->hour & 0xF0) >> 4) * 10 + (t->hour & 0x0F);
    if ((t->hour & 0x80) != 0) // if the high bit is set
    {
        // convert to 24-hour format by subtracting 12 from the hours
        t->hour = ((t->hour & 0x7F) + 12) % 24;
    }
    
    t->day = ((t->day & 0xF0) >> 4) * 10 + (t->day & 0x0F);
    t->month = ((t->month & 0xF0) >> 4) * 10 + (t->month & 0x0F);
    t->year = ((t->year & 0xF0) >> 4) * 10 + (t->year & 0x0F);
}

void cmos_read_clock(struct clock *t){
    // Read the CMOS time registers
    out(CMOS_ADDRESS_PORT, 0x00);
    t->behind_second = in(CMOS_DATA_PORT);
    out(CMOS_ADDRESS_PORT, 0x02);
    t->behind_minute = in(CMOS_DATA_PORT);
    out(CMOS_ADDRESS_PORT, 0x04);
    t->behind_hour = in(CMOS_DATA_PORT);
    out(CMOS_ADDRESS_PORT, 0x07);
    t->day = in(CMOS_DATA_PORT);
    out(CMOS_ADDRESS_PORT, 0x08);
    t->month = in(CMOS_DATA_PORT);
    out(CMOS_ADDRESS_PORT, 0x09);
    t->year = in(CMOS_DATA_PORT);
    out(CMOS_ADDRESS_PORT, 0x32);
    t->year |= in(CMOS_DATA_PORT) << 8;

    t->front_hour = ((t->behind_hour & 0xF0) >> 4) & 0x0F;
    t->behind_hour = t->behind_hour & 0x0F;
    t->front_minute = ((t->behind_minute & 0xF0) >> 4)  & 0x0F;
    t->behind_minute = t->behind_minute & 0x0F;
    t->front_second = ((t->behind_second & 0xF0) >> 4)  & 0x0F;
    t->behind_second = t->behind_second & 0x0F;
}