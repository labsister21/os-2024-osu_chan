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

// void cwd(){

//     char path[256];

//     //mengosongkan array path
//     for(int i = 0; i < 256; i++){
//         path[i] = '\0';
//     }

//     struct ClusterBuffer cl = {0};

//     int8_t retcode;

//     struct FAT32DriverRequest request = {
//         .buf = &cl,
//         .parent_cluster_number = current_working_directory_stat,
//         .buffer_size = 0,
//     };

//     memcpy(request.name, current_working_directory_name_stat, 8);
    
//     struct FAT32DirectoryTable dir_table = {0};

//     struct FAT32DirectoryEntry empty_entry = {0};

//     for (uint32_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
//     {
//             dir_table.table[i] = empty_entry;
//     }

//     request.buf = &dir_table;

// }