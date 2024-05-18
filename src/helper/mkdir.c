#include "mkdir.h"
#include "../header/stdlib/string.h"


void mkdir(char * dir_name){

    int8_t retcode;

    struct FAT32DriverRequest request = {   
        .parent_cluster_number = current_working_directory_stat,
        .buffer_size = 0,
    };

    memcpy(request.name, dir_name, 8);
    memcpy(request.nama_bapak, current_working_directory_name_stat, 8);

    syscall(2, (uint32_t)&request, (uint32_t)&retcode, 0);

    if (retcode == 0)
    {
            put("Berhasil Menulis Directory\n", WHITE);
    }
    else if(retcode == 1)
    {
            put("Maaf Directory Dengan Nama Yang Sama Sudah Ada\n", WHITE);
    }
    else{
            put("Gagal Menulis Directory\n", WHITE);
    }
}