#include <stdint.h>
#include "header/filesystem/fat32.h"
#include "header/stdlib/string.h"

void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx) {
    __asm__ volatile("mov %0, %%ebx" : /* <Empty> */ : "r"(ebx));
    __asm__ volatile("mov %0, %%ecx" : /* <Empty> */ : "r"(ecx));
    __asm__ volatile("mov %0, %%edx" : /* <Empty> */ : "r"(edx));
    __asm__ volatile("mov %0, %%eax" : /* <Empty> */ : "r"(eax));
    // Note : gcc usually use %eax as intermediate register,
    //        so it need to be the last one to mov
    __asm__ volatile("int $0x30");
}

size_t strlen(char *string) {
    size_t i = 0;
    while (string[i] != '\0')
        i++;
    return i;
}

// 0 sama, 1 beda
uint8_t strcmp(char *s1, char *s2) {
    size_t i = 0;
    if (strlen(s1) == strlen(s2)) {
        while (s1[i] != '\0') {
            if (s1[i] != s2[i])
                return 1;
            i++;
        }
        return 0;
    }
    return 1;
}

void put (char* buf, uint8_t color) {
    int i = 0;
    while (buf[i] != '\0')
        i++;
    syscall (5, (uint32_t) buf, i, color);
}

int main(void) {
    char buf;
    while(true){
        syscall(7, 0, 0, 0);
        put(&buf, 40);
    }
    return 0;
}

