#include "ls.h"
#include "../header/stdlib/string.h"


void cat(char * nama){
    
    int8_t retcode;

    char string_sep_dot[12][128];

    //mengosongkan sek
    for(int i = 0; i < 12; i++){

        for(int j = 0; j < 128; j++){
            string_sep_dot[i][j] = '\0';
        }
    }

    int many_substring = strparse(nama, string_sep_dot, ".");

    //berarti file 
    if(many_substring > 1){
        
        if(strlen(string_sep_dot[0]) > 8){
            put("Gomen, OSu_chan harusnya tidak punya nama file yang lebih dari 8 karakter :(\n", WHITE);
            return;
        }

        if(strlen(string_sep_dot[1]) > 3){
            put("Gomen, OSu_chan harusnya tidak punya ekstensi yang lebih dari 3 karakter :(\n", WHITE);
            return;
        }

        struct ClusterBuffer cbuf[8];

        struct FAT32DriverRequest request = {
            .buf = &cbuf,
            .parent_cluster_number = current_working_directory_stat,
            .buffer_size = sizeof(cbuf),
        };

        memcpy(request.name, string_sep_dot[0], 8);
        memcpy(request.ext, string_sep_dot[1], 3);
        
        syscall(0, (uint32_t)&request, (uint32_t)&retcode, 0);

        if (retcode == 0){

            struct ClusterBuffer isi[8];

            memcpy(isi, request.buf, sizeof(isi));

            for (int i = 0; i < 8; i++) {

                put((char *)isi[i].buf, WHITE);
                
            }
        }
        else if (retcode == 1){

            put("Itu Bukan File Ya\n", WHITE);

        } else if (retcode == 2){

            put("Gomen, Buffer Tidak Cukup\n", WHITE);

        } else if (retcode == 3) {

            put("File Tidak Ditemukan\n", WHITE);

        } else {
            
            put("Ada Masalah Dengan Directory\n", WHITE);
        }
    //ini buat folder
    }else{

        if(strlen(string_sep_dot[0]) > 8){
            put("Gomen, OSu_chan harusnya tidak punya nama folder yang lebih dari 8 karakter :(\n", WHITE);
            put("User san, cat juga gak bisa dipake buat foler\n", WHITE);
            return;
        }

        put("Gomen user san, cat gak bisa dipake buat foler\n", WHITE);        
    }
}