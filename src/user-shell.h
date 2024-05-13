#ifndef _USERSHELL_
#define _USERSHELL_

#include <stdint.h>
#include "header/filesystem/fat32.h"
#include "header/stdlib/string.h"

#define WHITE       0b1111

void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx);

void put(char* str, uint8_t color);

void parseCommand(char* buf);

bool isValidCharacter(char random_char);

void PWD(char *path, uint32_t curr_dir);

void loadingScreen();

#endif