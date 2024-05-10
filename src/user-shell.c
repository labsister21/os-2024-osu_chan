#include <stdint.h>
#include "header/filesystem/fat32.h"
#include "header/stdlib/string.h"


#define WHITE       0b1111

uint32_t current_directory = ROOT_CLUSTER_NUMBER;

void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx) {
    __asm__ volatile("mov %0, %%ebx" : /* <Empty> */ : "r"(ebx));
    __asm__ volatile("mov %0, %%ecx" : /* <Empty> */ : "r"(ecx));
    __asm__ volatile("mov %0, %%edx" : /* <Empty> */ : "r"(edx));
    __asm__ volatile("mov %0, %%eax" : /* <Empty> */ : "r"(eax));
    // Note : gcc usually use %eax as intermediate register,
    //        so it need to be the last one to mov
    __asm__ volatile("int $0x30");


}



// void updateDirectoryTable(uint32_t cluster_number) {
//     syscall(6, (uint32_t) &dir_table, cluster_number, 0x0);
// }

// void printCWD(char* path_str, uint32_t current_dir) {
//     // Intantiate vars and length vars
//     int pathlen = 0;
//     int nodecount = 0;
//     char nodeIndex [10][64];

//     // Biasakan untuk clear dulu
//     clear(path_str, 128);
//     for (int i = 0; i < 10; i++) {
//         clear(nodeIndex[i], 64);
//     }

//     if (current_dir == ROOT_CLUSTER_NUMBER) {
//         path_str[pathlen++] = '/';
//         put (path_str, WHITE);
//         return;
//     }
    
//     // Loop sampe parentnya ROOT
//     uint32_t parent = current_dir;
//     path_str[pathlen++] = '/';
//     while (parent != ROOT_CLUSTER_NUMBER) {
//         // Isi dir_table dengan isi dari cluster sekarang
//         updateDirectoryTable(parent);

//         // Ambil parentnya
//         parent = (uint32_t) ((dir_table.table[0].cluster_high << 16) | dir_table.table[0].cluster_low);
        
//         // Masukin namanya ke list
//         memcpy(nodeIndex[nodecount], dir_table.table[0].name, strlen(dir_table.table[0].name));
//         nodecount++;
//     }

//     // Iterate back to get the full pathstr
//     for (int i = nodecount - 1; i >= 0; i--) {
//         for (size_t j = 0; j < strlen(nodeIndex[i]); j++) {
//             path_str[pathlen++] = nodeIndex[i][j];
//         } 
        
//         if (i > 0) {
//             path_str[pathlen++] = '/';
//         }
//     }

//     put (path_str, WHITE);
// }

void put(char* str, uint8_t color) {
    syscall(6, (uint32_t) str, strlen(str), color);
}

void putn(char* str, uint8_t color, int n) {
    syscall(6, (uint32_t) str, n, color);
}

int inputparse (char *args_val, int args_info[128][2]) {
    // Declare the vars
    int nums = 0;

    // Process to count the args, initialize 0
    int i = 0;
    int j = 0;
    int k = 0;

    bool endWord = true;
    bool startWord = true;
    int countchar = 0;
    int len = strlen(args_val);

    // Iterate all over the chars
    // Ignore blanks at first
    while (args_val[i] == ' ' && i < len) {
        i++;
    }

    // While belum eof
    while (i < len) {
        // Ignore blanks
        while (args_val[i] == ' ' && args_val[i] != 0x0A) {
            if (!endWord) {
                k = 0;
                j++;
                endWord = true;
            }
            startWord = true;
            i++;
        }

        // Return the number of args
        if (args_val[i] == 0x0A) {
            return nums;
        }

        // Out then it is not the end of the word
        endWord = false;

        // Process other chars
        if (startWord) {
            nums++;
            countchar = 0;
            args_info[j][k] = i;
            startWord = false;
            k++;
        }

        countchar++;
        args_info[j][k] = countchar;
        i++; // Next char
    }

    char temp = i + '0';
    put(&temp, WHITE);
    return nums;
}

void screen() {
    put("  ___   ___  _   _         ___  _  _  ___  _  _\n", WHITE); 
    put(" / _ \\ / __|| | | |       / __|| || |/   \\| \\| |\n", WHITE);
    put("| (_) |\\__ \\| |_| |      | (__ | __ || - || .  |\n", WHITE);
    put(" \\___/ |___/ \\___/        \\___||_||_||_|_||_|\\_|\n", WHITE);

}

int main(void) {
    screen();

    char args[2048];
    int args_info[128][2];
    char path[2048];
    syscall(4, (uint32_t) args, 2048, 0x0);
    syscall(8, 0, 0, 0);


    while (true) {
        clear(args, 2048);
        for (int i = 0; i < 128; i++) {
            clear(args_info[i], 2);
        }

        clear(path, 2048);

        put("os2024-OSu_chan\0", WHITE);
        put(":", WHITE);
        put("/", WHITE);
        // printCWD(path, current_directory);
        put("$ ", WHITE);

        syscall(4, (uint32_t) args, 2048, 0);
        // put("hehe1", WHITE);

        // jumlah input
        int args_count = inputparse(args, args_info);
        // put("hehe2", WHITE);

        if(args_count != 0) {
            if ((memcmp(args + *(args_info)[0], "cd", 2) == 0) && ((*(args_info))[1] == 2)) {
                // cd(args, args_info, args_count);
                put("Ini cd\n", WHITE);
            } else if ((memcmp(args + *(args_info)[0], "ls", 2) == 0) && ((*(args_info))[1] == 2)) {
                // ls(args, args_info, args_count);
                put("Ini ls\n", WHITE);
            } else if ((memcmp(args + *(args_info)[0], "mkdir", 5) == 0)&& ((*(args_info))[1] == 5)) {
                // mkdir(args, args_info, args_count);
                put("Ini mkdir\n", WHITE);
            } else if ((memcmp(args + *(args_info)[0], "cat", 3) == 0)&& ((*(args_info))[1] == 3)) {
                // cat(args, args_info, args_count);
                put("Ini cat\n", WHITE);
            } else if ((memcmp(args + *(args_info)[0], "cp", 2) == 0)&& ((*(args_info))[1] == 2)) {
                // cp(args, args_info, args_count);
                put("Ini cp\n", WHITE);
            } else if ((memcmp(args + *(args_info)[0], "rm", 2) == 0)&& ((*(args_info))[1] == 2)) {
                // rm(args, args_info, args_count);
                put("Ini rm\n", WHITE);
            } else if ((memcmp(args + *(args_info)[0], "mv", 2) == 0)&& ((*(args_info))[1] == 2)) {
                // mv(args, args_info, args_count);
                put("Ini mv\n", WHITE);
            } else if ((memcmp(args + *(args_info)[0], "whereis", 7) == 0)&& ((*(args_info))[1] == 7)) {
                // whereis(args, args_info, args_count);
                put("Ini whereis\n", WHITE);
            } else if ((memcmp(args + *(args_info)[0], "clear", 5) == 0)&& ((*(args_info))[1] == 5)) {
                if (args_count > 1) {
                    put("clear: too many arguments\n", WHITE);
                } else {
                    syscall(8, 0, 0, 0);
                }
            } else {
                for (char i = 0; i < (*(args_info))[1]; i++) {
                    putn(args + (*(args_info))[0] + i, WHITE, 1);
                }
                put(": command not found\n", WHITE);
            }
        }
    }

    return 0;
}