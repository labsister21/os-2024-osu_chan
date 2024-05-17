#ifndef _PWD_H
#define _PWD_H

#include "../header/filesystem/fat32.h"
#include "../header/stdlib/string.h"
#include "../header/text/framebuffer.h"

#define WHITE       0b1111



struct CurrentWorkingDirectory
{
    uint32_t clusters_stack[CLUSTER_MAP_SIZE];
    char dir_names[CLUSTER_MAP_SIZE][8];
    int top;
} __attribute__((packed));


void put(char* str, uint8_t color);
void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx);
int8_t read_path(char *relative_path, struct CurrentWorkingDirectory *cwd, char *to_find);
int8_t separate_filename(char* filename, char* name, char* ext);


#endif