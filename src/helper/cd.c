#include "cd.h"
#include "../header/stdlib/string.h"



void cd(char * dir_name){

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

        if(memcmp(dir_name, "..", 2) == 0){

            uint32_t new_dir =  dir_table.table[1].cluster_high << 16 | dir_table.table[1].cluster_low;

            current_working_directory_stat = new_dir;

            for(int j = 0; j < 8; j++){

                current_working_directory_name_stat[j] = dir_table.table[1].name[j];

            }

            put("Berhasil Berpindah ke Directory Bapak\n", WHITE);

        }

        else
        
        {
            for(int i = 0; i < 64; i++){
        
                if(memcmp(dir_name, dir_table.table[i].name, 8) == 0){
                        
                    if (dir_table.table[i].attribute != ATTR_SUBDIRECTORY){
                        put(dir_name, WHITE);
                        put(" Bukan Directory Sir\n", WHITE);
                        return;
                    }

                    else{

                        uint32_t cur_dir = dir_table.table[i].cluster_high << 16 | dir_table.table[i].cluster_low;

                        current_working_directory_stat = cur_dir;

                        for(int j = 0; j < 8; j++){
                            current_working_directory_name_stat[j] = dir_table.table[i].name[j];
                        }

                        put("Berhasil masuk ke directory ", WHITE);
                        put(dir_name, WHITE);
                        put("\n", WHITE);

                        return;   

                    }

                }
            }

            put("Tidak ada directory bernama ", WHITE);
            put(dir_name, WHITE);
            put("\n", WHITE);
            return;
        }
    }
    else if(retcode == 1){
        put("Itu Bukan Directory\n", WHITE);
    }
    else{
        put("Ada Masalah Di Directory\n", WHITE);
    }
}