#include <stdint.h>
#include "header/filesystem/fat32.h"
#include "header/stdlib/string.h"
#include "user-shell.h"
#include "helper/clear.h"

uint32_t curr_dir = ROOT_CLUSTER_NUMBER;
char curr_dir_name[8] = {"root\0\0\0\0"};
struct FAT32DirectoryTable directory_table;

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

bool isValidCharacter(char random_char) {
    char validChars[] = {
        '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
        '-', '=', '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y',
        'u', 'i', 'o', 'p', '[', ']', 'a', 's', 'd', 'f', 'g', 
        'h', 'j', 'k', 'l', ';', '\'', '`','\\', 'z', 'x', 'c', 
        'v', 'b', 'n', 'm', ',', ' ','.', '/', '*'};

    for (int i = 0; i < 51 ; i++){
        if(validChars[i] == random_char){
            return true;
            break;
        }   
    }

    return false;
}


void PWD(char *path, uint32_t curr_dir){
    clear(path, 2048);
    int len_path = 0;

    if(curr_dir == ROOT_CLUSTER_NUMBER){
        path[len_path] = '/';
        len_path += 1;
        put(path, WHITE);
        return;
    }

    int nodecount = 0;
    char nodeIndex [10][64];

    for (int i = 0; i < 10; i++) {
        clear(nodeIndex[i], 64);
    }
    
    uint32_t parent = curr_dir;
    path[len_path++] = '/';
    while (parent != ROOT_CLUSTER_NUMBER) {

        syscall(9, (uint32_t) &directory_table, ROOT_CLUSTER_NUMBER, 0);

        parent = (uint32_t) ((directory_table.table[0].cluster_high << 16) | directory_table.table[0].cluster_low);
        
        memcpy(nodeIndex[nodecount], directory_table.table[0].name, strlen(directory_table.table[0].name));
        nodecount++;
    }

    for (int i = nodecount - 1; i >= 0; i--) {
        for (int j = 0; j < strlen(nodeIndex[i]); j++) {
           path[len_path++] = nodeIndex[i][j];
        } 
        
        if (i > 0) {
           path[len_path++] = '/';
        }
    }

    put (path, WHITE);
}

//parser masih banyak bug, rada ngeri
// void parseCommand(char* buf){

//     //kalau langsung enter
//     if(!isValidCharacter(buf[0])){
//         return;
//     }


//     char arg1[8];
//     clear(arg1, 8);

//     char arg2[8];
//     clear(arg2, 8);

//     char arg3[8];
//     clear(arg2, 8);

//     // deleteSpace(buf);

//     // // int validation;
//     // int start_index = -1;
//     // int last_arg_index;

//     // for(int i = 0; i < 2048; i++){
//     //     if(buf[i] != '\0'){
//     //         start_index = i;
//     //         break;
//     //     }

//     // }

//     // if(start_index == -1){
//     //     return;
//     // }

//     // for(int j = start_index; j < 2048; j++){
//     //     if(buf[j] == '\0'){
//     //         last_arg_index = j - 1;
//     //         break;
//     //     }
//     // }

//     // // entah kenapa kalau lebih besar dari 8 itu ada
//     // // karakter aneh yang masuk ke array
//     // if(((last_arg_index - start_index) + 1) > 8){
//     //     put("Command Tidak Valid :(\n", WHITE);
//     //     return;
//     // } 

//     // int m = 0;
//     // for(int l = start_index; l < (last_arg_index + 1) ; l++){
//     //     arg1[m] = buf[l];
//     //     m += 1;
//     // }

//     // m = 0;

//     if (memcmp(arg1, "clear", 5) == 0){
//         clear_command();
//     }
//     else if (memcmp(arg1, "mkdir", 5) == 0)
//     {
//         put("ini mkdir\n", WHITE); 

//         // int start_index2;
//         // int last_arg_index2;

//         // for(int i = last_arg_index + 1; i < 2048; i++){
//         //     if(buf[i] != '\0'){
//         //         start_index2 = i;
//         //         break;
//         //     }
//         // }

//         // for(int j = start_index2 ; j < 2048; j++){
//         //     if(buf[j] == '\0'){
//         //         last_arg_index2 = j - 1;
//         //         break;
//         //     }
//         // }  

//         // if(((last_arg_index2 - start_index2) + 1) > 8){
//         //     put("Nama directory nya nggak boleh lebih dari 8 karakter :(\n", WHITE);
//         //     return;
//         // } 

//         // if(!isValidCharacter(buf[start_index2])){
//         //     put("Command Tidak Valid :(\n", WHITE);
//         //     return;
//         // }

//         // m = 0;
//         // for(int k = start_index2; k < (last_arg_index2 + 1);k++){
//         //     arg2[m] = buf[k];
//         //     m += 1;
//         // } 

//         // m = 0;

//         // struct FAT32DriverRequest request = {
//         //     .parent_cluster_number = curr_dir,
//         //     .buffer_size = 0,
//         // };

//         // memcpy(request.name, arg2, 8);
//         // syscall(2, (uint32_t)&request, (uint32_t)&validation, 0);

//         // syscall(10, 0, 0, 0);
//         // clear(arg1, 8);
//         // clear(arg2, 8);
//         // clear(buf, 2048);

//         // if (validation == 0)
//         // {
//         //     put("Directory Berhasil Dibuat\n", WHITE);
//         //     return;
//         // }
//         // else if(validation == 1){
//         //     put("Directory Sudah Ada Bang\n", WHITE);
//         //     return;
//         // }
//         // else
//         // {
//         //     put("Directory Gagal Dibuat\n", WHITE);
//         //     return;
//         // }
//     }
//     else if(memcmp(arg1, "ls", 2) == 0){
//         put("ini ls\n", WHITE);
//         // struct FAT32DriverRequest request = {
//         //     .parent_cluster_number = curr_dir,
//         //     .buffer_size = 0,
//         // };
        
//         return;
//     }
//     else
//     {   
//         if(isValidCharacter(arg1[0])){
//             put("Command Tidak Valid :(\n", WHITE);
//         }
//     }
// }


void parseCommand(char *buf){

    if(!isValidCharacter(buf[0])){
        return;
    }

    int validation;

    int len_buf = 0;

    char arg1[8];
    char arg2[8];
    // char arg3[8];

    //start
    int start_arg_1 = -1;
    int start_arg_2 = -1;
    // int start_arg_3 = -1;

    //last
    int last_arg_1;
    int last_arg_2;

    //kosongin isi
    clear(arg1, 8);
    clear(arg2, 8);
    // clear(arg3, 8);


    for(int i = 0; i < 2048; i++){
        if(buf[i] == '\0'){
            break;
        }
        len_buf++;
    }

    if(len_buf == 0){
        return;
    }

    // if(len_buf == 5){
    //     put("len_buf = 4\n", WHITE);
    // }


    for(int j = 0; j < len_buf; j++){
        if(buf[j] != ' '){
            start_arg_1 = j;
            break;
        }
    }

    // if(start_arg_1 == 0){
    //     put("start_arg_1 = 0\n", WHITE);
    // }

    if (start_arg_1 == -1){
        return;
    }

    for(int k = start_arg_1; k < len_buf; k++){
        if(buf[k] == ' '){
            last_arg_1 = k - 1;
            break;
        }

        if(k == (len_buf - 1)){
            last_arg_1 = len_buf - 1;
        }
    }

    // if(last_arg_1 == 4){
    //     put("last_arg_1 = 4\n", WHITE);
    // }
    

    if( (last_arg_1 - start_arg_1 + 1) > 8){
        put("Command Tidak Valid\n", WHITE);
        return;
    }

    int counter = 0;
    for(int l = start_arg_1; l < (last_arg_1 + 1); l++){
        arg1[counter] = buf[l];
        counter++;
    }

    counter = 0;
    
    // put(arg1, WHITE);
    // put("\n", WHITE);

    if(memcmp(arg1, "clear", 5) == 0){
         clear_command();
    }
    else if(memcmp(arg1, "mkdir", 5) == 0){
        // put("ini mkdir\n", WHITE);

        if(last_arg_1 + 1 == len_buf){
            put("Command Tidak Valid\n", WHITE);
            return;
        }

        for(int m = (last_arg_1 + 1); m < len_buf; m++){
            if(buf[m] != ' '){
                start_arg_2 = m;
                break;
            }
        }

        if(start_arg_2 == -1){
            put("Command Tidak Valid\n", WHITE);
            return;
        }

        for(int o = start_arg_2; o < len_buf; o++){
            if(buf[o] == ' '){
                last_arg_2 = o - 1;
                break;
            }

            if(o == (len_buf - 1)){
                last_arg_2 = len_buf - 1;
            }
        }

        if((last_arg_1 + 1) < len_buf){
            for(int p = last_arg_2 + 1; p < len_buf; p++){
                if(buf[p] != ' '){
                    put("Command Tidak Valid\n", WHITE);
                    return;
                }
            }
        }

        if( (last_arg_2 - start_arg_2 + 1) > 8){
            put("Nama Folder Tidak Boleh Lebih dari 8 Huruf\n", WHITE);
            return;
        }

        int counter2 = 0;

        for(int l = start_arg_2; l < (last_arg_2 + 1); l++){
            arg2[counter2] = buf[l];
            counter2++;
        }

        counter2 = 0;

        struct FAT32DriverRequest request = {
            .parent_cluster_number = curr_dir,
            .buffer_size = 0,
        };

        memcpy(request.name, arg2, 8);
        syscall(2, (uint32_t)&request, (uint32_t)&validation, 0);

        clear(arg1, 8);
        clear(arg2, 8);

        if (validation == 0)
        {
            put("Directory Berhasil Dibuat\n", WHITE);
            return;
        }
        else if(validation == 1){
            put("Directory Sudah Ada Bang\n", WHITE);
            return;
        }
        else
        {
            put("Directory Gagal Dibuat\n", WHITE);
            return;
        }
    }
    else if(memcmp(arg1, "ls", 2) == 0){

        if((last_arg_1 + 1) < len_buf){
            for(int p = last_arg_1 + 1; p < len_buf; p++){
                if(buf[p] != ' '){
                    put("Command Tidak Valid\n", WHITE);
                    return;
                }
            }
        }
        
        //bikin request
        struct FAT32DriverRequest request2 = {
            .parent_cluster_number = curr_dir,
            .buffer_size = 0,
        };

        memcpy(request2.name, curr_dir_name, 8);

        struct FAT32DirectoryTable dir_table;

        struct FAT32DirectoryEntry empty_entry = {0};

        for (uint32_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
        {
            dir_table.table[i] = empty_entry;
        }

        request2.buf = &dir_table;

        syscall(1, (uint32_t)&request2, (uint32_t)&validation, 0);        
        
        if(validation == 0){

            for (int i = 2; i < 64; i++)
            {
                if (dir_table.table[i].name[0] == '\0')
                {
                    break;
                }

                for (int j = 0; j < 8; j++)
                {
                    if (dir_table.table[i].name[j] == '\0')
                    {
                        break;
                    }
                }

                put(dir_table.table[i].name, WHITE);

                if (!(dir_table.table[i].ext[0] == '\0'))
                {
                    put(".", WHITE);
                    put(dir_table.table[i].ext, WHITE);
                }
                put("\n", WHITE);
            }

        }
        else{
            put("Directory Kosong\n", WHITE);
        }
        return;
    }
    else{
        if(isValidCharacter(arg1[0])){
            put("Command Tidak Valid\n", WHITE);
        }
    }
}


void loading_Screen(){
put("++++++++++++#**%@%*++++++++++++++++\n", WHITE);
put("+++++++++++%@@@@@%@@@#+++++++++++++\n", WHITE);
put("+++++++++++@@@@@@@#+*@@@%++++++++++\n", WHITE);
put("++++++++++#@@@%@%+%@@#+%@@@#+++++++\n", WHITE);
put("+++++++++*@@+%@@@@@@#++++%@@@%+++++\n", WHITE);
put("++++++++@@#+%@@@@%++++++++*@@@@#+++\n", WHITE);
put("++++++%@@*+%@@@@@++++++++++*@@@@@++\n", WHITE);
put("++++%@@@++@@@@@@%+++++++++++*@@@@@*\n", WHITE);
put("++#@@@%*%@@@@@@@@%+++++++++++@@@@@%\n", WHITE);
put("+%@@@@*%@@@@@@@@@@*++++++++++%@@@@@\n", WHITE);
put("%@@@@%++++%@@#%@@%+#+++++++++@@@@@@\n", WHITE);
put("@@@@@%++++%@@*+@@@**+++++++*@@@@@@@\n", WHITE);
put("@@@@@@@*++#@%++*@@#+++++++%%@@@@@@*\n", WHITE);
put("%@@@@#@@@@@@#+++#@@*+++++@**@@@@%++\n", WHITE);
put("*@@@@%+#++#@%+++++@@%++++%**@@#++++\n", WHITE);
put("++*@@@%+++*@%++++++*@%+++++#@++++++\n", WHITE);
put("+++++*@%+++%%++++++++@@++++*%++++++\n", WHITE);
put("+++++++*+++#@*+++++++#@@+++++++++++\n", WHITE);
put("+++++++++++@@*++++++*@@*+++++++++++\n", WHITE);
put("         Welcome To OSu_chan       \n", WHITE);
put("     Click Enter to Continue Nyan  \n", WHITE);
}


int main(void) {
    char args[2048];
    char path[2048];

    loading_Screen();
    syscall(4, (uint32_t) args, 2048, 0);
    clear_command();

    while (true) {
        clear(args, 2048);
        clear(path, 2048);

        put("OS2024@OSu_chan\0", WHITE);
        put(":", WHITE);
        PWD(path, curr_dir);
        put("$ ", WHITE);
        syscall(4, (uint32_t) args, 2048, 0);

        parseCommand(args);

    }

    return 0;
}