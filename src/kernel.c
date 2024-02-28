#include <stdint.h>
#include <stdbool.h>
#include "header/cpu/gdt.h"
// #include "gdt.c"
#include "header/kernel-entrypoint.h"

// void kernel_setup(void) {
//     uint32_t a;
//     uint32_t volatile b = 0x0000BABE;
//     __asm__("mov $0xCAFE0000, %0" : "=r"(a));
//     // load_gdt(&_gdt_gdtr);
//     while (true) b += 1;
// }
void kernel_setup(void) {
    load_gdt(&_gdt_gdtr);
    while (true);
}
