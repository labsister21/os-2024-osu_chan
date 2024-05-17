#ifndef _MKDIR_H
#define _MKDIR_H

#include "pwd.h"
#include "../header/filesystem/fat32.h"
#include "../header/text/framebuffer.h"
#include "../header/stdlib/string.h"
#include "../header/stdlib/string.h"

void mkdir(struct CurrentWorkingDirectory cwd, char *folder_name);

#endif