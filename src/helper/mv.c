#include "mv.h"
#include "../header/stdlib/string.h"


void mv(char * dir_name_source, char * dest_name){

    char source_name[8];
    char destination_name[8];

    //kosongin
    clear(source_name, 8);
    clear(destination_name, 8);

    char string_sep_dot[12][128];
    char string_sep_dot2[12][128];

    //ngosongin 
    for(int i = 0; i < 12; i++){
        for(int j = 0; j < 128; j++){ 
            string_sep_dot[i][j] = '\0';
        }
    }

    //ngosongin lagi
    for(int i = 0; i < 12; i++){
        for(int j = 0; j < 128; j++){ 
            string_sep_dot2[i][j] = '\0';
        }
    }
    
    int many_substring = strparse(dir_name_source, string_sep_dot, ".");
    int many_substring2 = strparse(dest_name, string_sep_dot2, ".");

    if(many_substring > 1){
        put("\nPemindahan File Belum Diimplementasikan\n\n", WHITE);
        return;
    }

    if(many_substring2 > 1){
        put("\nTujuan Seharusnya Folder\n\n", WHITE);
        return;
    }

    // put(string_sep_dot[0], WHITE);
    // put("\n", WHITE);

    if(strlen(string_sep_dot[0]) > 8){
        put("\nOSu_chan tidak punya nama folder sumber sepanjang itu\n\n", WHITE);
        return;
    }

    if(strlen(string_sep_dot2[0]) > 8){
        put("\nOSu_chan tidak punya nama folder tujuan sepanjang itu\n\n", WHITE);
        return;
    }

    //salin ke char ukuran 8
    memcpy(source_name, string_sep_dot[0], strlen(string_sep_dot[0]));
    memcpy(destination_name, string_sep_dot2[0], 8);

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

    if(retcode != 0){
        put("\nAda Masalah Dengan Directory\n\n", WHITE);
        return;
    }

    uint32_t source_cluster_number;

    for(int i = 2; i < 64; i++){

        if(memcmp(dir_table.table[i].name, source_name, 8) == 0){

            // put("Source Name ada\n", WHITE);

            source_cluster_number = dir_table.table[i].cluster_high << 16 | dir_table.table[i].cluster_low;

            // char source_cluster = source_cluster_number + '0';

            // put(&source_cluster, WHITE);

            // put("\n", WHITE);

            break;

        }
    }


    for(int i = 2; i < 64; i++){

        if(memcmp(dir_table.table[i].name, destination_name, 8) == 0){

            // put("Destination Name ada\n", WHITE);

            uint32_t temp = dir_table.table[i].cluster_high << 16 | dir_table.table[i].cluster_low;

            // char destination_cluster = current_working_directory_stat + '0';

            // put(&destination_cluster, WHITE);

            // put("\n", WHITE);
            
            // put(current_working_directory_name_stat, WHITE);

            // put("\n", WHITE);

            struct ClusterBuffer cl2 = {0};

            int8_t retcode2;

            struct FAT32DriverRequest request2 = {
                .buf = &cl2,
                .parent_cluster_number = temp,
                .buffer_size = 0,
                .cluster_move = source_cluster_number,
            };

            memcpy(request2.name, source_name, 8);

            struct FAT32DirectoryTable dir_table2 = {0};

            struct FAT32DirectoryEntry empty_entry2 = {0};

            for (uint32_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
            {
                dir_table2.table[i] = empty_entry2;
            }

            request2.buf = &dir_table2;

            syscall(9, (uint32_t)&request2, (uint32_t)&retcode2, 0); 

            struct ClusterBuffer cl3 = {0};

            int8_t retcode3;

            uint32_t tempx = (uint32_t) current_working_directory_stat;

            char nama_penghapusan[8];
            clear(nama_penghapusan, 8);

            memcpy(nama_penghapusan, string_sep_dot[0], strlen(string_sep_dot[0]));

            struct FAT32DriverRequest request3 = {
                .buf = &cl3,
                .parent_cluster_number = tempx,
            };

            memcpy(request3.name, nama_penghapusan, 8);

            struct FAT32DirectoryTable dir_table3 = {0};

            struct FAT32DirectoryEntry empty_entry3 = {0};

            for (uint32_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
            {
                    dir_table3.table[i] = empty_entry3;
            }

            request3.buf = &dir_table3;

            syscall(10, (uint32_t)&request3, (uint32_t)&retcode3, 0); 

            if(retcode2 == 0){
                put("\nBerhasil memindah folder ", WHITE);
                put(source_name, WHITE);
                put(" ke ", WHITE);
                put(destination_name, WHITE);
                put("\n\n", WHITE);

            }

            else if(retcode2 == 1){
                put("\nDi Folder ", WHITE);
                put(destination_name, WHITE);
                put(" Sudah Ada Folder Dengan Nama Yang Sama\n\n", WHITE);
                return;
            }
            else if(retcode2 == 2){
                put("\nFolder Tujuan Sudah Penuh\n\n", WHITE);
                return;   
            }
            else{
                put("\n", WHITE);
                put(destination_name, WHITE);
                put(" Bukan Folder\n\n", WHITE);
                return;
            }
            // put("aman\n", WHITE);
            // // put(dir_table2.table[0].name, WHITE);
            // // put(dir_table2.table[1].name, WHITE);

            // // put("\n", WHITE);

            break;
        }
    }
}