#ifndef _FIND_H
#define _FIND_H

#include "pwd.h"
#include "../header/filesystem/fat32.h"
#include "../header/text/framebuffer.h"
#include "../header/stdlib/string.h"


void find(char *nama);

void dfs(char * nama, char * ext, bool * check, uint32_t *current_location, char *current_name, int * code, int * type, char * cur_ext);

#endif