#include <stdint.h>
#include "header/filesystem/fat32.h"
#include "header/stdlib/string.h"
#include "helper/pwd.h"
#include "helper/mkdir.h"
#include "helper/cd.h"


int main(void) {

    char command[12][128];
    char buf[2048];

    struct CurrentWorkingDirectory cwd = {
        .clusters_stack = {2},
        .dir_names = {"root\0\0\0\0"},
        .top = 0,
    };

    while(true){

        //mengosongkan buf
        for(int i = 0; i < 2048; i++){
            buf[i] = '\0';
        }

        put("IF2024@OSu_chan:", WHITE);
        syscall(4, (uint32_t)buf, 2048, 0);

        int args = strparse(buf, command, " ");

        if(args > 0){

            put(command[0], WHITE);

            put("\n", WHITE);

            if(memcmp(command[0], "cd", 2) == 0){
                if (args >= 2){
                    cd(&cwd, command[1]);
                }
            }
            else if(memcmp(command[0], "ls", 2) == 0){
                put("ini ls\n", WHITE);
            }
            else if(memcmp(command[0], "mkdir", 2) == 0){
                put("ini mkdir\n", WHITE);
                if (args >= 2){
                    mkdir(cwd, command[1]);
                }
            }
            else if(memcmp(command[0], "clear", 2) == 0){
                syscall(8, 0, 0, 0);
            }
            else{
                put("Command Tidak Valid\n", WHITE);
            }
        } 
    }

    return 0;
}