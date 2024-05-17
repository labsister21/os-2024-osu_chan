#ifndef _CD_H
#define _CD_H

#include "pwd.h"
#include "../header/filesystem/fat32.h"
#include "../header/text/framebuffer.h"
#include "../header/stdlib/string.h"

void cd(struct CurrentWorkingDirectory *cwd, char *dir_name);

#endif