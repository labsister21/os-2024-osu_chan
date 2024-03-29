#include <stdint.h>
#include <stdbool.h>
#include "header/cpu/gdt.h"
// #include "gdt.c"
#include "header/kernel-entrypoint.h"
#include "header/text/framebuffer.h"

void kernel_setup(void) {
    // uint32_t a;
    // uint32_t volatile b = 0x0000BABE;
    // __asm__("mov $0xCAFE0000, %0" : "=r"(a));
    load_gdt(&_gdt_gdtr);
    // framebuffer_clear();
    // framebuffer_write(3, 8,  'H', 0, 0xF);
    // framebuffer_write(3, 9,  'a', 0, 0xF);
    // framebuffer_write(3, 10, 'i', 0, 0xF);
    // framebuffer_write(3, 11, '!', 0, 0xF);
    // framebuffer_set_cursor(3, 10);
    // framebuffer_write(4, 8,  'H', 0, 0xF);
    // framebuffer_write(4, 9,  'a', 0, 0xF);
    // framebuffer_write(4, 10, 'i', 0, 0xF);
    // framebuffer_write(4, 11, '!', 0, 0xF);
    // framebuffer_set_cursor(4, 10);
    while(true);
    // while (true) b += 1;
}
// void kernel_setup(void)
// {
//     load_gdt(&_gdt_gdtr);
//     while (true);
// }
