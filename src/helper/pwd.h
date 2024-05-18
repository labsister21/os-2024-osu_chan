#ifndef _PWD_H
#define _PWD_H

#include "../header/filesystem/fat32.h"
#include "../header/stdlib/string.h"
#include "../header/text/framebuffer.h"

#define WHITE       0b1111

extern uint32_t current_working_directory_stat;
extern char current_working_directory_name_stat[8];

void put(char* str, uint8_t color);

void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx);

void cwd();

void reverse_path(const char *path, char *reversed_path);

#endif