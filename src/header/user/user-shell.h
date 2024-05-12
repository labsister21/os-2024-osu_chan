#ifndef _USER_SHELL
#define _USER_SHELL

#include <stdint.h>
#include "../filesystem/fat32.h"
#include "../stdlib/string.h"

#define WHITE       0b1111
#define RED         0b1100
#define GREEN       0b1010
#define BLUE        0b1001

uint32_t current_directory = ROOT_CLUSTER_NUMBER;
struct FAT32DirectoryTable dir_table;


void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx);

int inputparse (char *args_val, int args_info[128][2]);

void put(char* str, uint8_t color);

bool isPathAbsolute(char* args_val, int (*args_info)[2], int args_pos);

int findEntryName(char* name);
void updateDirectoryTable(uint32_t cluster_number);

void printCWD(char* path_str, uint32_t current_dir);

void putn(char* str, uint8_t color, int n);

void screen();

#endif