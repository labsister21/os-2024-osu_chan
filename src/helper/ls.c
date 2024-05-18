#include "ls.h"
#include "../header/stdlib/string.h"



void ls(){
    
    struct ClusterBuffer cl = {0};

    int8_t retcode;

    struct FAT32DriverRequest request = {
        .buf = &cl,
        .parent_cluster_number = current_working_directory_stat,
        .buffer_size = 0,
    };

    memcpy(request.name, current_working_directory_name_stat, 8);

    struct FAT32DirectoryTable dir_table = {0};

    struct FAT32DirectoryEntry empty_entry = {0};

    for (uint32_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
            dir_table.table[i] = empty_entry;
    }

    request.buf = &dir_table;

    syscall(1, (uint32_t)&request, (uint32_t)&retcode, 0);   

    if(retcode == 0){

        for (int i = 2; i < 64; i++)
        {
            if (dir_table.table[i].name[0] == '\0')
                {
                        break;
                }

            for (int j = 0; j < 8; j++)
                {
                    if (dir_table.table[i].name[j] == '\0')
                        {
                            break;
                        }
                }

            put(dir_table.table[i].name, WHITE);

            if (!(dir_table.table[i].ext[0] == '\0'))
                {
                    put(".", WHITE);
                    put(dir_table.table[i].ext, WHITE);
                }

            put("\n", WHITE);
        }
    }   
    else{

        put("Ada Masalah dengan Directory\n", WHITE);

    }
}