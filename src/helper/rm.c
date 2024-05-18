#include "rm.h"
#include "../header/stdlib/string.h"

void rm(char *nama){

    char string_sep_dot[12][128];

    //mengosongkan sek
    for(int i = 0; i < 12; i++){

        for(int j = 0; j < 128; j++){
            string_sep_dot[i][j] = '\0';
        }
    }

    int many_substring = strparse(nama, string_sep_dot, ".");

    //kalau lebih besar dari 1 , maksudnya ya 2 dia itu file
    //punya ekstensin misal nguawor.txt
    if(many_substring > 1){
 
        int8_t retcode_delete_file;

        struct FAT32DriverRequest request_delete_file = {
            .parent_cluster_number = current_working_directory_stat,
        };

        // put("nama file ", WHITE);
        // put(string_sep_dot[0], WHITE);
        // put("\n", WHITE);

        // put("ekstensi ", WHITE);
        // put(string_sep_dot[1], WHITE);
        // put("\n", WHITE);

        memcpy(request_delete_file.name, string_sep_dot[0], 8);
        memcpy(request_delete_file.ext, string_sep_dot[1], 3);

        syscall(3, (uint32_t)&request_delete_file, (uint32_t)&retcode_delete_file, 0);  

        if(retcode_delete_file == 0){
            put("Berhasil Menghapus File ", WHITE);
            put(nama, WHITE);
            put("\n", WHITE);
        }

        else if(retcode_delete_file == 1){
            put("File ", WHITE);
            put(nama, WHITE);
            put(" Tidak Ditemukan Nyan\n", WHITE);
        }
        else{
            put("Gagal Melakukan Penghapusan, Mungkin Ada Masalah Nyan\n", WHITE);
        }
    }
    else{

        if(memcmp(string_sep_dot[0], "shell", 5) == 0){
            put("Shell Gak Boleh Dihapus Nyan\n", WHITE);
            return;
        }

        int8_t retcode_delete_folder;

        struct FAT32DriverRequest request_delete_folder = {
            .parent_cluster_number = current_working_directory_stat,
        };

        // put("nama folder ", WHITE);
        // put(string_sep_dot[0], WHITE);
        // put("\n", WHITE);

        memcpy(request_delete_folder.name, string_sep_dot[0], 8);
        
        for(int i = 0; i < 3; i++){
            request_delete_folder.ext[i] = '\0';
        }

        syscall(3, (uint32_t)&request_delete_folder, (uint32_t)&retcode_delete_folder, 0);  

        if(retcode_delete_folder == 0){
            put("Berhasil Menghapus Folder ", WHITE);
            put(nama, WHITE);
            put("\n", WHITE);
        }

        else if(retcode_delete_folder == 1){
            put("Folder ", WHITE);
            put(nama, WHITE);
            put(" Tidak Ditemukan\n", WHITE);
        }
        else if(retcode_delete_folder == 2){
            put("Folder ", WHITE);
            put(nama, WHITE);
            put(" Tidak Kosong Nyan\n", WHITE);  
        }
        else{
            put("Gagal Melakukan Penghapusan, Mungkin Ada Masalah Nyan\n", WHITE);
        }
    }      
}