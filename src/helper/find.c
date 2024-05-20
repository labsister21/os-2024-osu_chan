#include "find.h"
#include "../header/stdlib/string.h"

void find(char *nama){

    char string_sep_dot[12][128];

    //mengosongkan sek
    for(int i = 0; i < 12; i++){

        for(int j = 0; j < 128; j++){
            string_sep_dot[i][j] = '\0';
        }
    }

    int many_substring = strparse(nama, string_sep_dot, ".");

    //kalau teks
    if(many_substring > 1){
        
        if(strlen(string_sep_dot[0]) > 8){
            put("Gomen, OSu_chan harusnya tidak punya nama file yang lebih dari 8 karakter :(\n", WHITE);
            return;
        }

        if(strlen(string_sep_dot[1]) > 3){
            put("Gomen, OSu_chan harusnya tidak punya ekstensi yang lebih dari 3 karakter :(\n", WHITE);
            return;
        }

        bool cek = false;

        uint32_t cur_dir = ROOT_CLUSTER_NUMBER;

        char cur_name[10];

        char temp_ext[5];

        clear(temp_ext, 5);

        clear(cur_name, 10);

        memcpy(cur_name, "root\0\0\0\0\0\0", 8);

        memcpy(temp_ext, "\0\0\0\0\0", 5);

        int code = 0;

        int tipe = -1;

        dfs(string_sep_dot[0], string_sep_dot[1], &cek, &cur_dir, cur_name, &code, &tipe, temp_ext);

        if(code == 1){
            
            put("\nGomen, Ada Masalah Dengan Proses Pembukaan Directory\n\n", WHITE);

            return;

        }

        if(cek == false){

            put("\nGomen, File/Folder Yang Anda Cari Tidak Ditemukan\n\n", WHITE);

            return;
        }

        if(tipe == 1){
            cwd_find(cur_dir, cur_name);  
            put("\n", WHITE);

        }
        else{
            put("\n", WHITE);
            cwd_find(cur_dir, cur_name);
            put(nama, WHITE);
            put("\n\n", WHITE);
        }
    }
    else{
        
        if(strlen(string_sep_dot[0]) > 8){
            put("Gomen, OSu_chan harusnya tidak punya nama folder yang lebih dari 8 karakter :(\n", WHITE);
            return;
        }

        bool cek2 = false;

        uint32_t cur_dir2 = ROOT_CLUSTER_NUMBER;

        char cur_name2[10];

        char temp_ext2[5];

        clear(temp_ext2, 5);

        clear(cur_name2, 10);

        memcpy(cur_name2, "root\0\0\0\0\0\0", 8);

        memcpy(temp_ext2, "\0\0\0\0\0", 5);

        int code2 = 0;

        int tipe2 = -1;

        clear(string_sep_dot[1], 128);

        dfs(string_sep_dot[0], string_sep_dot[1], &cek2, &cur_dir2, cur_name2, &code2, &tipe2, temp_ext2);

        if(code2 == 1){
            
            put("\nGomen, Ada Masalah Dengan Proses Pembukaan Directory\n\n", WHITE);

            return;

        }

        if(cek2 == false){

            put("\nGomen, File/Folder Yang Anda Cari Tidak Ditemukan\n\n", WHITE);

            return;
        }

        
        if(tipe2 == 1){
            cwd_find(cur_dir2, cur_name2);
            put("\n", WHITE);   
        }
        else{
            cwd_find(cur_dir2, cur_name2);
            put(nama, WHITE);
            put("\n", WHITE);
        }
    }
}


void dfs(char * nama, char * ext, bool * check, uint32_t *current_location, char *current_name, int * code, int * type, char * cur_ext){

    if(memcmp(nama, current_name, strlen(current_name)) == 0){
        
        *check = true;
        *type = 1;
        
    }

    struct ClusterBuffer cl = {0};

    int8_t retcode;

    struct FAT32DriverRequest requestx = {
        .buf = &cl,
        .parent_cluster_number = *current_location,
    };

    memcpy(requestx.name, current_name, 8);

    struct FAT32DirectoryTable dir_table = {0};

    struct FAT32DirectoryEntry empty_entry = {0};

    for (uint32_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
            dir_table.table[i] = empty_entry;
    }

    requestx.buf = &dir_table;

    syscall(1, (uint32_t)&requestx, (uint32_t)&retcode, 0);

    if(retcode != 0){
        *code = 1;
        // 1 i gagal cik
        return;
    }

    for(uint32_t i = 2; i < 64; i++){

        if(!(dir_table.table[i].name[0] == '\0')){

            // put("Apakah Masuk Sini\n", WHITE);
            
            // put(dir_table.table[i].name, WHITE);
            // put("\n", WHITE);

            if(dir_table.table[i].attribute == ATTR_SUBDIRECTORY){
                
                // put(dir_table.table[i].name, WHITE);
                // put("\n", WHITE);

                clear(current_name, 8);

                memcpy(current_name, dir_table.table[i].name, 8); 

                uint32_t temp_alamat = dir_table.table[i].cluster_high << 16 | dir_table.table[i].cluster_low;

                *current_location = temp_alamat;

                char temp_ext3[5];

                clear(temp_ext3, 5);

                memcpy(cur_ext, temp_ext3, 3);

                dfs(nama, ext, check, current_location, current_name, code, type, cur_ext);

                
                if((*check) == true || (*code) == 1){
                    break;                   
                }

            }

            else{

                if(memcmp(dir_table.table[i].name, nama, strlen(nama)) == 0){


                    if(memcmp(dir_table.table[i].ext, ext, 3) == 0){
                        *check = true;
                        //type 0 kalau file
                        uint32_t temp_alamat2 = dir_table.table[0].cluster_high << 16 | dir_table.table[0].cluster_low;

                        *current_location = temp_alamat2;
        
                        clear(current_name, 8);

                        memcpy(current_name, dir_table.table[0].name, 8); 

                        *type = 0;
                    }

                    if((*check) == true || (*code) == 1){
                        break;                   
                    }
                }

            }
        }
    }

    return;
}