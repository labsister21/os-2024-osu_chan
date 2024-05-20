#include <stdint.h>
#include "header/filesystem/fat32.h"
#include "header/stdlib/string.h"
#include "helper/pwd.h"
#include "helper/mkdir.h"
#include "helper/cd.h"
#include "helper/ls.h"
#include "helper/rm.h"
#include "helper/cat.h"
#include "helper/find.h"
#include "helper/cp.h"
#include "helper/mv.h"

int main(void) {

    char command[12][128];
    char buf[2048];


    while(true){

        //mengosongkan buf
        for(int i = 0; i < 2048; i++){
            buf[i] = '\0';
        }

        put("IF2024@OSu_chan:", WHITE);
        // cwd();
        put("$ ", WHITE);
        syscall(4, (uint32_t)buf, 2048, 0);

        int args = strparse(buf, command, " ");

        if(args > 0){

            if(memcmp(command[0], "cd", 2) == 0){
                if(args < 2){
                    put("Command Tidak Valid\n", WHITE);
                }
                else if(args == 2){
                    if(strlen(command[1]) > 8){
                        put("Gomen, Tidak Ada Folder Sepanjang itu atau Mungkin Kamu Masukin Nama File\n", WHITE);
                    }
                    else{
                        cd(command[1]);
                    }
                }
                else{
                    put("Command Tidak Valid\n", WHITE);
                }
            }
            else if(memcmp(command[0], "cat", 3) == 0){
                if(args < 2){
                    put("Command Tidak Valid\n", WHITE);
                }
                else if(args == 2){          
                    if(memcmp(command[1], "..", 2) == 0){
                        put("Bakaa, Mau Ngapain Nyan!!!!!\n", WHITE);
                    }else if(memcmp(command[1], "shell", 5) == 0){
                        put("Gomen, kalau gak boleh buka itu xixixi\n", WHITE);
                    }
                    else{
                        cat(command[1]);
                    }
                }
                else{
                    put("Command Tidak Valid\n", WHITE);
                }
            }
            else if(memcmp(command[0], "find", 4) == 0){
                if(args < 2){
                    put("Command Tidak Valid\n", WHITE);
                }
                else if(args == 2){       
                    find(command[1]);
                }
                else{
                    put("Command Tidak Valid\n", WHITE);
                }
            }
            else if(memcmp(command[0], "cp", 2) == 0){
                if(args < 3){
                    put("Command Tidak Valid\n", WHITE);
                }
                else if(args == 3){       
                    cp(command[1], command[2]);
                }
                else{
                    put("Command Tidak Valid\n", WHITE);
                }
            }
            else if(memcmp(command[0], "mv", 2) == 0){
                if(args < 3){
                    put("Command Tidak Valid\n", WHITE);
                }
                else if(args == 3){       
                    mv(command[1], command[2]);
                }
                else{
                    put("Command Tidak Valid\n", WHITE);
                }
            }
            else if(memcmp(command[0], "rm", 2) == 0){
                if(args < 2){
                    put("Command Tidak Valid\n", WHITE);
                }
                else if(args == 2){          
                    if(memcmp(command[1], "..", 2) == 0){
                        put("Bakaa, Mau Ngapain Nyan!!!!!\n", WHITE);
                    }else{
                        rm(command[1]);
                    }
                }
                else{
                    put("Command Tidak Valid\n", WHITE);
                }
            }
            else if(memcmp(command[0], "ls", 2) == 0){
                if(args > 1){
                    put("Command Tidak Valid\n", WHITE);
                }
                else {
                    ls();
                }
            }
            else if(memcmp(command[0], "mkdir", 5) == 0){
                if(args < 2){
                    put("Command Tidak Valid\n", WHITE);
                }
                else if(args == 2){
                    if(strlen(command[1]) > 8){
                        put("Nama Folder Terlalu Panjang\n", WHITE);
                    }
                    else{
                        mkdir(command[1]);
                    }
                }
                else{
                    put("Command Tidak Valid\n", WHITE);
                }
            }
            else if(memcmp(command[0], "clear", 5) == 0){
                syscall(8, 0, 0, 0);
            }
            else{
                put("Command Tidak Valid\n", WHITE);
            }
        } 
    }

    return 0;
}
