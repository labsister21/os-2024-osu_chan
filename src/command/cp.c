
// File : cp.c
// Contains the implementation of functions needed to process cp command

#include "cp.h"
#include "user-shell.h"
#include "stdlib/string.h"

void copy(char* args_val, int (*args_info)[2], int args_count) {
    /* Searches if the destination exists and if it is a file or directory.
       Returns 1 if it is a file, 0 if it is a directory, -1 if it is not found */

    // Variables to keep track the currently visited directory
    uint32_t dest_search_directory_number = ROOT_CLUSTER_NUMBER;
    char destName[8] = {'\0','\0','\0','\0','\0','\0','\0','\0'};
    char destExt[3] = {'\0','\0','\0'};

    // Variables for parsing the arguments
    int posName = (*(args_info + args_count-1))[0];
    int lenName = 0;
    int index = posName;
    int entry_index = -1;

    int posEndArgs = (*(args_info + args_count-1))[0] + (*(args_info + args_count-1))[1];
    bool endOfArgs = (posName+lenName-1 == posEndArgs);
    bool endWord = true;
    bool newFileFound = false;

    int errorCode = 0;

    // If path is not absolute, set the currently visited directory to current working directory
    if (!isPathAbsolute(args_val, args_info, args_count-1)) {
        dest_search_directory_number = current_directory;
    }

    // Get the directory table of the visited directory
    updateDirectoryTable(dest_search_directory_number);

    // Start searching for the directory to make 
    while (!endOfArgs) {
        // If current char is not '/', process the information of word. Else, process the word itself
        if (memcmp(args_val + index, "/", 1) != 0 && index != posEndArgs) {
            // If word already started, increment the length. Else, start new word
            if (!endWord) {
                lenName++;
            }
            else {
                // If there is a new word after non-existent directory, set an error code and stop parsing
                if (newFileFound) {
                    newFileFound = false;
                    if (errorCode == 5) {
                        errorCode = 1;
                    }
                    else {
                        errorCode = 4;
                    }
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
                    // Cek extension
                    int i = 0;
                    while (i < lenName && memcmp(".", args_val + posName + i, 1) != 0) {
                        i++;
                    }
                    if (i >= lenName) {
                        errorCode = 3;
                        endOfArgs = true;
                    } else if (lenName-i-1 > 3) {
                        errorCode = 3;
                        endOfArgs = true;
                    } else {
                        clear(destName, 8);
                        clear(destExt,3);
                        memcpy(destName, args_val + posName, i);
                        if (*(args_val + posName + i + 1) != 0x0A) {
                            memcpy(destExt, args_val + posName + i + 1, lenName-i-1);
                        }
                        entry_index = findEntryName(destName);
                        if (entry_index == -1) {
                            newFileFound = true;
                        }
                        else {
                            if (dir_table.table[entry_index].attribute == ATTR_SUBDIRECTORY) {
                                dest_search_directory_number = (int) ((dir_table.table[entry_index].cluster_high << 16) | dir_table.table[entry_index].cluster_low);
                                updateDirectoryTable(dest_search_directory_number);
                            }
                            else {
                                newFileFound = true;
                                errorCode = 5;
                            }
                        }
                    }
                    endWord = true;
                }
                else if (lenName == 2 && memcmp(args_val + posName, "..", 2) == 0) {
                    dest_search_directory_number = (int) ((dir_table.table[0].cluster_high << 16) | dir_table.table[0].cluster_low);
                    updateDirectoryTable(dest_search_directory_number);
                }
                else {
                    clear(destName, 8);
                    clear(destExt,3);
                    // Cek extension
                    int i = 0;
                    while (i < lenName && memcmp(".", args_val + posName + i, 1) != 0) {
                        i++;
                    }
                    if (i < lenName) { // Jika ada extension
                        if (lenName-i-1 > 3) { // Jika extension lebih dari 3 karakter
                            errorCode = 3;
                            break;
                        }   
                        memcpy(destName, args_val + posName, i);
                        if (*(args_val + posName + i + 1) != 0x0A) {
                            memcpy(destExt, args_val + posName + i + 1, lenName-i-1);
                        }
                    } else {
                        memcpy(destName, args_val + posName, lenName);
                    }
                    entry_index = findEntryName(destName);
                    if (entry_index == -1) {
                        newFileFound = true;
                    }
                    else {
                        if (dir_table.table[entry_index].attribute == ATTR_SUBDIRECTORY) {
                            dest_search_directory_number = (int) ((dir_table.table[entry_index].cluster_high << 16) | dir_table.table[entry_index].cluster_low);
                            updateDirectoryTable(dest_search_directory_number);
                        }
                        else {
                            newFileFound = true;
                            errorCode = 5;
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

    if (errorCode == 3 || errorCode ==  4) {
        put("cp: Destination not valid\n", RED);
        return;
    }
    else if (args_count > 3 && newFileFound) { // Jika lebih dari satu file yang dicopy, maka harus dimasukkan ke dalam folder
        put("cp: target '", RED);
        putn(args_val + (*(args_info + args_count-1))[0], RED, (*(args_info + args_count-1))[1]);
        put("' is not a directory\n", RED);
        return;
    }

    // Read each files that need to be copied
    for (int j=1; j<args_count-1; j++) {
        // Variables to keep track the currently visited directory
        uint32_t src_search_directory_number = ROOT_CLUSTER_NUMBER;
        char srcName[8] = {'\0','\0','\0','\0','\0','\0','\0','\0'};
        char srcExt[3] = {'\0','\0','\0'};

        // Variables for parsing the arguments
        posName = (*(args_info + j))[0];
        lenName = 0;
        index = posName;
        entry_index = -1;

        posEndArgs = (*(args_info + j))[0] + (*(args_info + j))[1];
        endOfArgs = (posName+lenName-1 == posEndArgs);
        endWord = true;
        bool srcNewFileFound = false;

        errorCode = 0;

        // If path is not absolute, set the currently visited directory to current working directory
        if (!isPathAbsolute(args_val, args_info, j)) {
            src_search_directory_number = current_directory;
        }

        // Get the directory table of the visited directory
        updateDirectoryTable(src_search_directory_number);

        // Start searching for the directory to make 
        while (!endOfArgs) {
            // If current char is not '/', process the information of word. Else, process the word itself
            if (memcmp(args_val + index, "/", 1) != 0 && index != posEndArgs) {
                // If word already started, increment the length. Else, start new word
                if (!endWord) {
                    lenName++;
                }
                else {
                    // If there is a new word after non-existent directory, set an error code and stop parsing
                    if (srcNewFileFound) {
                        srcNewFileFound = false;
                        if (errorCode == 5) {
                            errorCode = 1;
                        }
                        else {
                            errorCode = 4;
                        }
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
                        // Periksa extension
                        int i = 0;
                        while (i < lenName && memcmp(".", args_val + posName + i, 1) != 0) {
                            i++;
                        }
                        clear(srcName, 8);
                        clear(srcExt,3);
                        if (i >= lenName) {
                            errorCode = 3;
                            endOfArgs = true;
                        } 
                        else if (lenName-i-1 > 3) {
                            errorCode = 3;
                            endOfArgs = true;
                        } 
                        else {
                            memcpy(srcName, args_val + posName, i);
                            if (*(args_val + posName + i + 1) != 0x0A) {
                                memcpy(srcExt, args_val + posName + i + 1, lenName-i-1);
                            }
                            entry_index = findEntryName(srcName);
                            if (entry_index == -1) {
                                srcNewFileFound = true;
                            }
                            else {
                                if (dir_table.table[entry_index].attribute == ATTR_SUBDIRECTORY) {
                                    src_search_directory_number = (int) ((dir_table.table[entry_index].cluster_high << 16) | dir_table.table[entry_index].cluster_low);
                                    updateDirectoryTable(src_search_directory_number);
                                }
                                else {
                                    srcNewFileFound = true;
                                    errorCode = 5;
                                }
                            }
                        }
                        endWord = true;
                    }
                    else if (lenName == 2 && memcmp(args_val + posName, "..", 2) == 0) {
                        src_search_directory_number = (int) ((dir_table.table[0].cluster_high << 16) | dir_table.table[0].cluster_low);
                        updateDirectoryTable(src_search_directory_number);
                    }
                    else {
                        clear(srcName, 8);
                        clear(srcExt,3);
                        // Periksa extension
                        int i = 0;
                        while (i < lenName && memcmp(".", args_val + posName + i, 1) != 0) {
                            i++;
                        }
                        if (i < lenName) { // Jika ada extension
                            if (lenName-i-1 > 3) { // Jika extension lebih dari 3 karakter
                                errorCode = 3;
                                break;
                            }
                            memcpy(srcName, args_val + posName, i);
                            if (*(args_val + posName + i + 1) != 0x0A) {
                                memcpy(srcExt, args_val + posName + i + 1, lenName-i-1);
                            }
                        } else {
                            memcpy(srcName, args_val + posName, lenName);
                        }
                        entry_index = findEntryName(srcName);
                        if (entry_index == -1) {
                            srcNewFileFound = true;
                        }
                        else {
                            if (dir_table.table[entry_index].attribute == ATTR_SUBDIRECTORY) {
                                src_search_directory_number = (int) ((dir_table.table[entry_index].cluster_high << 16) | dir_table.table[entry_index].cluster_low);
                                updateDirectoryTable(src_search_directory_number);
                            }
                            else {
                                srcNewFileFound = true;
                                errorCode = 5;
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

        if (errorCode == 3 || errorCode ==  4) {
            put("cp: cannot stat '", RED);
            putn(args_val + (*(args_info + j))[0], RED, (*(args_info + j))[1]);
            put("': No such file or directory\n", RED);
            return;
        }
        else if (!srcNewFileFound) {
            put("cp: '", RED);
            putn(args_val + (*(args_info + j))[0], RED, (*(args_info + j))[1]);
            put("' is a directory\n", RED);
            return;
        }

        struct ClusterBuffer cbuf = {0};
        struct FAT32DriverRequest srcReq = {
            .buf = &cbuf,
            .name = "\0\0\0\0\0\0\0\0",
            .ext = "\0\0\0",
            .parent_cluster_number = src_search_directory_number,
            .buffer_size = CLUSTER_SIZE
        };
        memcpy(&(srcReq.name), srcName, 8);
        memcpy(&(srcReq.ext), srcExt, 3);
        int retCode;

        interrupt(0, (uint32_t) &srcReq, (uint32_t) &retCode, 0x0);

        if (retCode != 0) {
            put("cp: cannot stat '", RED);
            putn(args_val + (*(args_info + j))[0], RED, (*(args_info + j))[1]);
            switch (retCode) {
                case 1:
                    put("': Is a directory\n", RED);
                    return;
                case 2:
                    put("': Buffer size is not enough\n", RED);
                    return;
                case 3:
                    put("': No such file or directory\n", RED);
                    return;
                case -1:
                    put("': Unknown error\n", RED);
                    return;
            }
        } else {
            if (!newFileFound) {
                // Tujuan berupa direktori
                srcReq.parent_cluster_number = dest_search_directory_number;
                interrupt(3, (uint32_t) &srcReq, (uint32_t) &retCode, 0x0);
                interrupt(2, (uint32_t) &srcReq, (uint32_t) &retCode, 0x0);
                if (retCode != 0) {
                    put("cp: cannot copy '", RED);
                    putn(args_val + (*(args_info + j))[0], RED, (*(args_info + j))[1]);
                    switch (retCode) {
                    case 1:
                        put("': File exist\n", RED);
                        return;
                    case -1:
                        put("': Unknown error occured\n", RED);
                        return;
                    }
                }
            } else {
                // Tujuan berupa file
                struct FAT32DriverRequest destReq = {
                    .buf = &cbuf,
                    .name = "\0\0\0\0\0\0\0\0",
                    .ext = "\0\0\0",
                    .parent_cluster_number = dest_search_directory_number,
                    .buffer_size = CLUSTER_SIZE
                };
                memcpy(&(destReq.name), destName, 8);
                memcpy(&(destReq.ext), destExt, 3);
                interrupt(3, (uint32_t) &destReq, (uint32_t) &retCode, 0x0);
                interrupt(2, (uint32_t) &destReq, (uint32_t) &retCode, 0x0);
                if (retCode != 0) {
                    put("cp: cannot copy '", RED);
                    putn(args_val + (*(args_info + j))[0], RED, (*(args_info + j))[1]);
                    switch (retCode) {
                    case 1:
                        put("': File exist\n", RED);
                        return;
                    case -1:
                        put("': Unknown error occured\n", RED);
                        return;
                    }
                }
            }
        }
    }
}

void cp(char* args_val, int (*args_info)[2], int args_count) {
    if (args_count > 2) {
        copy(args_val, args_info, args_count);
    }
    else if (args_count == 2) {
        put("cp: missing destination file operand after '", RED);
        putn(args_val + (*(args_info + 1))[0], RED, (*(args_info + 1))[1]);
        put("'\n", RED); 
    }
    else {
        put("cp: missing file operand\n", RED);
    }
}