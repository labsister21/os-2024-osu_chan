#include "pwd.h"
#include "../header/stdlib/string.h"



uint32_t current_working_directory_stat = ROOT_CLUSTER_NUMBER;
char current_working_directory_name_stat[8] = {"root\0\0\0\0"};

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

void cwd(){

    char path[256];

    //mengosongkan array path
    for(int i = 0; i < 256; i++){
        path[i] = '\0';
    }

    uint32_t cur_dir_number = current_working_directory_stat;

    char cur_dir_name[8];

    for(int i = 0; i < 8; i++){

        cur_dir_name[i] = '\0';

    }

    memcpy(cur_dir_name, current_working_directory_name_stat, 8);

    struct FAT32DirectoryTable dir_table = {0};

    struct FAT32DirectoryEntry empty_entry = {0};

    for (uint32_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
            dir_table.table[i] = empty_entry;
    }

    int idx_start = 0;

    if(cur_dir_number == ROOT_CLUSTER_NUMBER){

        put("~", WHITE);

        return;
    }

    while(cur_dir_number != ROOT_CLUSTER_NUMBER){

        for(int i = 0; i < (int) strlen(cur_dir_name); i++){

            path[idx_start] = cur_dir_name[i];

            idx_start += 1;
        }

        path[idx_start] = '/';

        idx_start += 1;

        struct ClusterBuffer cl = {0};

        int8_t retcode;

        struct FAT32DriverRequest request = {

            .buf = &cl,
            .parent_cluster_number = cur_dir_number,
            .buffer_size = 0,

        };

        memcpy(request.name, cur_dir_name, 8);

        request.buf = &dir_table;

        syscall(1, (uint32_t)&request, (uint32_t)&retcode, 0);

        uint32_t temp_dir = dir_table.table[1].cluster_high << 16 | dir_table.table[1].cluster_low;

        cur_dir_number = temp_dir;

        memcpy(cur_dir_name, dir_table.table[1].name, 8);
    }

    char real_path[256];

    //mengosongkan array path
    for(int i = 0; i < 256; i++){

        real_path[i] = '\0';

    }

    reverse_path(path, real_path);

    put(real_path, WHITE);
}

void reverse_path(const char *path, char *reversed_path) {

    char temp[256];

    for(int i = 0; i < 256; i++){

        temp[i] = '\0';
    }

    char *tokens[256];

    for(int i = 0; i < 256; i++){

        *tokens[i] = '\0';
    }

    int count = 0;

    // Salin path ke temp untuk tokenisasi
    memcpy(temp, path, 256);

    // Tokenisasi string berdasarkan '/'
    char *token = strtok(temp, "/");
    while (token != NULL) {
        tokens[count++] = token;
        token = strtok(NULL, "/");
    }

    // Susun token dalam urutan terbalik
    int pos = 0;

    for (int i = count - 1; i >= 0; i--) {
        reversed_path[pos++] = '/';
        int len = strlen(tokens[i]);
        memcpy(reversed_path + pos, tokens[i], len);
        pos += len;
    }

    reversed_path[pos] = '\0'; 
}