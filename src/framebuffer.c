#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "header/text/framebuffer.h"
#include "header/stdlib/string.h"
#include "header/cpu/portio.h"

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
    out(0x3D4, 0x0A);
    out(0x3D5, (in(0x3D5) & 0xC0) | cursor_start);

    out(0x3D4, 0x0B);
    out(0x3D5, (in(0x3D5) & 0xE0) | cursor_end);
}

void disable_cursor()
{
    out(0x3D4, 0x0A);
    out(0x3D5, 0x20);
}

uint16_t get_cursor_position(void)
{
    uint16_t pos = 0;
    out(0x3D4, 0x0F);
    pos |= in(0x3D5);
    out(0x3D4, 0x0E);
    pos |= ((uint16_t)in(0x3D5)) << 8;
    return pos;
}

void framebuffer_set_cursor(uint8_t r, uint8_t c)
{
    // TODO : Implement
}

void framebuffer_write(uint8_t row, uint8_t col, char c, uint8_t fg, uint8_t bg)
{
    // TODO : Implement
}

void framebuffer_clear(void)
{
    // TODO : Implement -
}