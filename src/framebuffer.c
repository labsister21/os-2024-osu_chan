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
    uint16_t pos = r * 80 + c;

	out(CURSOR_PORT_CMD, 0x0F);
	out(CURSOR_PORT_DATA, (uint8_t) (pos & 0xFF));
	out(CURSOR_PORT_CMD, 0x0E);
	out(CURSOR_PORT_DATA, (uint8_t) ((pos >> 8) & 0xFF));  
}

void framebuffer_write(uint8_t row, uint8_t col, char c, uint8_t fg, uint8_t bg)
{
    uint16_t attrib = (bg << 4) | (fg & 0x0F);
    volatile uint16_t * where;
    where = (volatile uint16_t *)0xB8000 + (row * 80 + col) ;
    *where = c | (attrib << 8);
}

void framebuffer_clear(void)
{
    //ROW nya 25 cuy
    //COLUMNS nya 50 cuy
    for (int i = 0; i < 25; i++)
    {
        for (int j = 0; j < 80; j++)
        {
            framebuffer_write(i,j, 0x00, 0x07, 0x00);
        }
    }
    
}