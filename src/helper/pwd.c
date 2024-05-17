#include "pwd.h"
#include "../header/stdlib/string.h"

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

int8_t read_path(char *relative_path, struct CurrentWorkingDirectory *cwd, char *to_find)
{
    char dirs[12][128];
    int dir_cnt = strparse(relative_path, dirs, "/");

    uint32_t curr_dir_cluster = cwd->clusters_stack[cwd->top];

    struct FAT32DirectoryTable dir_table = {0};

    struct FAT32DriverRequest request = {
        .buf = &dir_table,
        .name = "\0\0\0\0\0\0\0\0",
        .ext = "\0\0\0",
        .parent_cluster_number = curr_dir_cluster,
        .buffer_size = CLUSTER_SIZE,
    };

    if (dir_cnt > 0)
    {
        for (int i = 0; i < dir_cnt - 1; i++)
        {
            if (memcmp("..", dirs[i], 2 ) == 0)
            {
                if (curr_dir_cluster != ROOT_CLUSTER_NUMBER && cwd->top != 0)
                {
                    cwd->top--;
                }
                else
                {
                    return 3; // path not found
                }
            }
            else if (dirs[i][0] != '\0')
            {
                int8_t retcode = -1;
                int found_cluster_number = 0;
                request.parent_cluster_number = cwd->clusters_stack[cwd->top];
                if (strlen(dirs[i]) > 8)
                {
                    return 3;
                }
                memcpy(request.name, dirs[i], 8);
                if (i != dir_cnt - 1) // must be directory
                {
                    syscall(1, (uint32_t)&request, (uint32_t)&retcode, (uint32_t)&found_cluster_number);
                    if (retcode == 0) // if found
                    {
                        curr_dir_cluster = found_cluster_number;
                        cwd->top++;
                        cwd->clusters_stack[cwd->top] = curr_dir_cluster;
                        memcpy(cwd->dir_names[cwd->top], dirs[i], 8);
                    }
                    else
                    {
                        return retcode;
                    }
                }
            }
        }
        if (memcmp("..", dirs[dir_cnt - 1], 2) == 0)
        {
            if (cwd->top > 0)
            {
                cwd->top--;
            }
        }
        else
        {
            memcpy(to_find, dirs[dir_cnt - 1], 12); // copies the name of the file/dir to look for
        }
        return 0; // success
    }

    return 3; // path not found
}

int8_t separate_filename(char *file_name, char *name, char *ext)
{

    // SEPERATE FILE NAME INTO NAME AND EXT
    const char *delim = ".";
    char *new_name;
    char *new_ext;

    new_name = strtok(file_name, delim);
    new_ext = strtok(NULL, delim);

    // IF NO EXTENSION, REPLACE EXTENSION WITH \0
    if (new_ext == NULL)
    {
        memcpy(new_ext, "\0\0\0", 3);
    }

    // CHECK IF NAME AND EXT OUTSIDE CONSTRAINTS
    if (strlen(new_name) > 8 || strlen(new_ext) > 3)
    {
        return 3; // OUT OF CONSTRAINTS
    }

    // RESET BUFFER
    memcpy(name, "\0\0\0\0\0\0\0\0", 8);
    memcpy(ext, "\0\0\0", 3);

    memcpy(name, new_name, strlen(new_name));
    memcpy(ext, new_ext, strlen(new_ext));

    return 0; // SUCCESS
}
