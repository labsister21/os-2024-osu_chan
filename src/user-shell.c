#include <stdint.h>
#include "header/filesystem/fat32.h"
#include "header/stdlib/string.h"


#define WHITE       0b1111

void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx) {
    __asm__ volatile("mov %0, %%ebx" : /* <Empty> */ : "r"(ebx));
    __asm__ volatile("mov %0, %%ecx" : /* <Empty> */ : "r"(ecx));
    __asm__ volatile("mov %0, %%edx" : /* <Empty> */ : "r"(edx));
    __asm__ volatile("mov %0, %%eax" : /* <Empty> */ : "r"(eax));
    // Note : gcc usually use %eax as intermediate register,
    //        so it need to be the last one to mov
    __asm__ volatile("int $0x30");


}

void put(char* str, uint8_t color) {
    syscall(6, (uint32_t) str, strlen(str), color);
}


int main(void) {
    char args[2048];

    while (true) {
        clear(args, 2048);

        put("os2024-OSu_chan\0", WHITE);
        put(":", WHITE);
        put("/", WHITE);
        put("$ ", WHITE);

        syscall(4, (uint32_t) args, 2048, 0);
    }

    return 0;
}