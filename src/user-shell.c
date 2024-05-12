#include <stdint.h>
#include "header/filesystem/fat32.h"
#include "header/stdlib/string.h"
#include "user-shell.h"
#include "helper/clear.h"

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

void parseCommand(char* buf){

    char arg1[8];
    clear(arg1, 8);
    deleteSpace(buf);

    int start_index;
    int last_arg_index;

    for(int i = 0; i < 2048; i++){
        if(buf[i] != '\0'){
            start_index = i;
            break;
        }
    }

    for(int j = start_index; j < 2048; j++){
        if(buf[j] == '\0'){
            last_arg_index = j - 1;
            break;
        }
    }

    if(((last_arg_index - start_index) + 1) > 8){
        put("Command Tidak Valid :(\n", WHITE);
    } 

    int m = 0;
    for(int l = start_index; l < (last_arg_index + 1) ; l++){
        arg1[m] = buf[l];
        m += 1;
    }


    if (memcmp(arg1, "clear", 5) == 0){
        clear_command();
    }
    else if (memcmp(arg1, "mkdir", 5) == 0)
    {
        put("ini mkdir\n", WHITE);   
    }
    else
    {   
        put("Command Tidak Valid\n", WHITE);
    }
}


int main(void) {
    char args[2048];

    while (true) {
        clear(args, 2048);

        put("os2024-OSu_chan\0", WHITE);
        put(":", WHITE);
        put("/", WHITE);

        put("$ ", WHITE);
        syscall(4, (uint32_t) args, 2048, 0);
        parseCommand(args);

    }

    return 0;
}