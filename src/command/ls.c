#include "ls.h"
#include "../header/user/user-shell.h"

void printDirectoryTable() {
    for (int i = 1; i < 63; i++) {
        if (dir_table.table[i].user_attribute == UATTR_NOT_EMPTY) {
            if (dir_table.table[i].name[7] != '\0') {
                putn(dir_table.table[i].name, BLUE, 8);
            }
            else {
                put(dir_table.table[i].name, BLUE);
            }
            if (dir_table.table[i].attribute != ATTR_SUBDIRECTORY && strlen(dir_table.table[i].ext) != 0) {
                put(".", BLUE);
                putn(dir_table.table[i].ext, BLUE, 3);
            }
            put("\n", BLUE);
        }
    }
    if (dir_table.table[63].user_attribute == UATTR_NOT_EMPTY) {
        if (dir_table.table[63].name[7] != '\0') {
            putn(dir_table.table[63].name, BLUE, 8);
        }
        else {
            put(dir_table.table[63].name, BLUE);
        }
        if (dir_table.table[63].attribute != ATTR_SUBDIRECTORY && strlen(dir_table.table[63].ext) != 0) {
            put(".", BLUE);
            putn(dir_table.table[63].ext, BLUE, 3);
        }
        put("\n", BLUE);
    }
}

void access(char* args_val, int (*args_info)[2], int args_pos) {
    // Variables to keep track the currently visited directory
    uint32_t search_directory_number = ROOT_CLUSTER_NUMBER;
    int oneArgFlag = args_pos;

    if (args_pos == -1) {
        search_directory_number = current_directory;
        updateDirectoryTable(search_directory_number);
        printDirectoryTable();
    }
    else {
        if (args_pos == 0) {
            args_pos++;
        }

        // Variables for parsing the arguments
        int posName = (*(args_info + args_pos))[0];
        int lenName = 0;
        int index = posName;
        int entry_index = -1;
        char* name = "\0\0\0\0\0\0\0\0";

        int posEndArgs = (*(args_info + args_pos))[0] + (*(args_info + args_pos))[1];
        bool endOfArgs = (posName+lenName-1 == posEndArgs);
        bool endWord = true;
        bool fileFound = false;
        bool directoryNotFound = false;

        int errorCode = 0;

        // If path is not absolute, set the currently visited directory to current working directory
        if (!isPathAbsolute(args_val, args_info, args_pos)) {
            search_directory_number = current_directory;
        }

        // Get the directory table of the visited directory
        updateDirectoryTable(search_directory_number);

        // Start searching for the directory to make 
        while (!endOfArgs) {
            // If current char is not '/', process the information of word. Else, process the word itself
            if (memcmp(args_val + index, "/", 1) != 0 && index != posEndArgs) {
                // If word already started, increment the length. Else, start new word
                if (!endWord) {
                    lenName++;
                }
                else {
                    if (fileFound && index != posEndArgs) {
                        errorCode = 1;
                        directoryNotFound = true;
                        fileFound = false;
                        endOfArgs = true;
                    }
                    else {
                        endWord = false;
                        posName = index;
                        lenName = 1;
                    }
                }
            }
            else {
                // Process the word
                if (!endWord) {
                    // If word length more than 8, set an error code and stop parsing. Else, check if the word exist as directory
                    if (lenName > 8) {
                        errorCode = 3;
                        directoryNotFound = true;
                        endOfArgs = true;
                    }
                    else if (lenName == 2 && memcmp(args_val + posName, "..", 2) == 0) {
                        search_directory_number = (int) ((dir_table.table[0].cluster_high << 16) | dir_table.table[0].cluster_low);
                        updateDirectoryTable(search_directory_number);
                    }
                    else {
                        clear(name, 8);
                        memcpy(name, args_val + posName, lenName);
                        entry_index = findEntryName(name);
                        if (entry_index == -1) {
                            directoryNotFound = true;
                            endOfArgs = true;
                        }
                        else {
                            if (dir_table.table[entry_index].attribute == ATTR_SUBDIRECTORY) {
                                search_directory_number =  (int) ((dir_table.table[entry_index].cluster_high << 16) | dir_table.table[entry_index].cluster_low);
                                updateDirectoryTable(search_directory_number);
                            }
                            else {
                                fileFound = true;
                            }
                        }
                    }
                    endWord = true;
                }
            }

            if (!endOfArgs) {
                if (index == posEndArgs) {
                    endOfArgs = true;
                }
                else {
                    index++;
                }
            }
        }

        if (directoryNotFound) {
            put("ls: cannot access '", RED);
            putn(args_val + (*(args_info + args_pos))[0], RED, (*(args_info + args_pos))[1]); 
            switch (errorCode) {
            case 1:
                put("': Not a directory\n", RED);
                break;
            case 3:
                put("': Directory name is too long\n", RED);
                break;
            default:
                put("': No such file or directory\n", RED);
                break;
            }
        }
        else {
            if (fileFound) {
                putn(args_val + (*(args_info + args_pos))[0], WHITE, (*(args_info + args_pos))[1]);
                put("\n", WHITE);
            }
            else { 
                if (oneArgFlag > 0) {
                    putn(args_val + (*(args_info + args_pos))[0], WHITE, (*(args_info + args_pos))[1]);
                    put(":\n", WHITE);
                }
                printDirectoryTable();
            }
        }
    }
}

void ls(char* args, int (*args_info)[2], int args_count) {
    if (args_count > 2) {
        for (int i = 1; i < args_count-1; i++) {
            access(args, args_info, i);
            put("\n", WHITE);
        }
        access(args, args_info, args_count - 1);
        put("\n", WHITE);
    }
    else if (args_count == 2) {
        access(args, args_info, 0);
    }
    else {
        access(args, args_info, -1);
    }
}