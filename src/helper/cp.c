#include "cp.h"
#include "../header/stdlib/string.h"


void cp(char * dir_name_source, char * dest_name){

    char source_name[8];
    char destination_name[8];

    //kosongin
    clear(source_name, 8);
    clear(destination_name, 8);

    char string_sep_dot[12][128];
    char string_sep_dot2[12][128];

    //ngosongin 
    for(int i = 0; i < 12; i++){
        for(int j = 0; j < 128; j++){ 
            string_sep_dot[i][j] = '\0';
        }
    }

    //ngosongin lagi
    for(int i = 0; i < 12; i++){
        for(int j = 0; j < 128; j++){ 
            string_sep_dot2[i][j] = '\0';
        }
    }
    
    int many_substring = strparse(dir_name_source, string_sep_dot, ".");
    int many_substring2 = strparse(dest_name, string_sep_dot2, ".");

    if(many_substring > 1){
        put("\nPemindahan File Belum Diimplementasikan\n\n", WHITE);
        return;
    }

    if(many_substring2 > 1){
        put("\nTujuan Seharusnya Folder\n\n", WHITE);
        return;
    }

    // put(string_sep_dot[0], WHITE);
    // put("\n", WHITE);

    if(strlen(string_sep_dot[0]) > 8){
        put("\nOSu_chan tidak punya nama folder sumber sepanjang itu\n\n", WHITE);
        return;
    }

    if(strlen(string_sep_dot2[0]) > 8){
        put("\nOSu_chan tidak punya nama folder tujuan sepanjang itu\n\n", WHITE);
        return;
    }

    //salin ke char ukuran 8
    memcpy(source_name, string_sep_dot[0], strlen(string_sep_dot[0]));
    memcpy(destination_name, string_sep_dot2[0], 8);

    struct ClusterBuffer cl = {0};

    int8_t retcode;

    struct FAT32DriverRequest request = {
        .buf = &cl,
        .parent_cluster_number = current_working_directory_stat,
        .buffer_size = 0,
    };

    memcpy(request.name, current_working_directory_name_stat, 8);


    struct FAT32DirectoryTable dir_table = {0};

    struct FAT32DirectoryEntry empty_entry = {0};

    for (uint32_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
                dir_table.table[i] = empty_entry;
    }

    request.buf = &dir_table;

    syscall(1, (uint32_t)&request, (uint32_t)&retcode, 0); 

    if(retcode != 0){
        put("\nAda Masalah Dengan Directory\n\n", WHITE);
        return;
    }

    uint32_t source_cluster_number;

    for(int i = 2; i < 64; i++){

        if(memcmp(dir_table.table[i].name, source_name, 8) == 0){

            // put("Source Name ada\n", WHITE);

            source_cluster_number = dir_table.table[i].cluster_high << 16 | dir_table.table[i].cluster_low;

            // char source_cluster = source_cluster_number + '0';

            // put(&source_cluster, WHITE);

            // put("\n", WHITE);

            break;

        }
    }


    for(int i = 2; i < 64; i++){

        if(memcmp(dir_table.table[i].name, destination_name, 8) == 0){

            // put("Destination Name ada\n", WHITE);

            uint32_t temp = dir_table.table[i].cluster_high << 16 | dir_table.table[i].cluster_low;

            char destination_cluster = current_working_directory_stat + '0';

            put(&destination_cluster, WHITE);

            put("\n", WHITE);
            
            put(current_working_directory_name_stat, WHITE);

            put("\n", WHITE);

            struct ClusterBuffer cl2 = {0};

            int8_t retcode2;

            struct FAT32DriverRequest request2 = {
                .buf = &cl2,
                .parent_cluster_number = temp,
                .buffer_size = 0,
                .cluster_move = source_cluster_number,
            };

            memcpy(request2.name, source_name, 8);

            struct FAT32DirectoryTable dir_table2 = {0};

            struct FAT32DirectoryEntry empty_entry2 = {0};

            for (uint32_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
            {
                dir_table2.table[i] = empty_entry2;
            }

            request2.buf = &dir_table2;

            syscall(9, (uint32_t)&request2, (uint32_t)&retcode2, 0); 

            if(retcode2 == 0){
                put("\nBerhasil mengcopy folder ", WHITE);
                put(source_name, WHITE);
                put(" ke ", WHITE);
                put(destination_name, WHITE);
                put("\n\n", WHITE);


                
                put(&destination_cluster, WHITE);

                put("\n", WHITE);
                
                put(current_working_directory_name_stat, WHITE);

                put("\n", WHITE);
                return;
            }

            else if(retcode2 == 1){
                put("\nDi Folder ", WHITE);
                put(destination_name, WHITE);
                put(" Sudah Ada Folder Dengan Nama Yang Sama\n\n", WHITE);
                return;
            }
            else if(retcode2 == 2){
                put("\nFolder Tujuan Sudah Penuh\n\n", WHITE);
                return;   
            }
            else{
                put("\n", WHITE);
                put(destination_name, WHITE);
                put(" Bukan Folder\n\n", WHITE);
                return;
            }
            // put("aman\n", WHITE);
            // // put(dir_table2.table[0].name, WHITE);
            // // put(dir_table2.table[1].name, WHITE);

            // // put("\n", WHITE);

            break;
        }
    }
}


// void cp(char *nama, char * tujuan){

//     //mov folder kosong ke folder lain
//     char string_sep_dot[12][128];

//     //mengosongkan sek
//     for(int i = 0; i < 12; i++){

//         for(int j = 0; j < 128; j++){ 
//             string_sep_dot[i][j] = '\0';
//         }
//     }

//     int many_substring = strparse(nama, string_sep_dot, ".");

//     char string_sep_dot2[12][128];

//     //mengosongkan sek
//     for(int i = 0; i < 12; i++){

//         for(int j = 0; j < 128; j++){
//             string_sep_dot2[i][j] = '\0';
//         }
//     }


//     int many_substring2 = strparse(tujuan, string_sep_dot2, ".");

//     uint32_t dest_cluster;

//     char dest_name[10];

//     uint32_t source_cluster;

//     char source_name[10];

//     //berarti folder
//     if(many_substring == 1){
        
//         struct ClusterBuffer cl = {0};

//         int8_t retcode;

//         struct FAT32DriverRequest request = {
//             .buf = &cl,
//             .parent_cluster_number = current_working_directory_stat,
//             .buffer_size = 0,
//         };

//         memcpy(request.name, current_working_directory_name_stat, 8);

//         struct FAT32DirectoryTable dir_table = {0};

//         struct FAT32DirectoryEntry empty_entry = {0};

//         for (uint32_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
//         {
//                 dir_table.table[i] = empty_entry;
//         }

//         request.buf = &dir_table;

//         syscall(1, (uint32_t)&request, (uint32_t)&retcode, 0); 

//         if(retcode != 0){
//             put("Ada Masalah Dengan Directory\n", WHITE);
//             return;
//         }

//         // put(dir_table.table[4].name, WHITE);

//         bool ifExist = false;

//         for(int i = 2; i < 64; i++){

//             if(memcmp(dir_table.table[i].name, string_sep_dot[0], strlen(string_sep_dot[0])) == 0){

//                 source_cluster = dir_table.table[i].cluster_high << 16 | dir_table.table[i].cluster_low;

//                 clear(source_name, 10);       

//                 memcpy(source_name, dir_table.table[i].name, strlen(dir_table.table[i].name));

//                 ifExist = true;

//                 break;                                           
//             }
//         }

//         if(!ifExist){
//             put("Folder ", WHITE);
//             put(string_sep_dot[0], WHITE);
//             put(" tidak ada\n", WHITE);
//             return;
//         }

//         if(many_substring2 == 1){

//             bool ifExist2 = false;

//             for(int i = 2; i < 64; i++){

//                 if(memcmp(dir_table.table[i].name, string_sep_dot2[0], strlen(string_sep_dot2[0])) == 0){

//                     dest_cluster = dir_table.table[i].cluster_high << 16 | dir_table.table[i].cluster_low;

//                     clear(dest_name, 10);       

//                     memcpy(dest_name, dir_table.table[i].name, strlen(dir_table.table[i].name));

//                     ifExist2 = true;

//                     break;                                           
//                 }
//             }

//             char cluster_cek = dest_cluster + '0';

//             put(&cluster_cek, WHITE);

//             if(!ifExist2){
//                 put("Folder ", WHITE);
//                 put(string_sep_dot2[0], WHITE);
//                 put(" tidak ada\n", WHITE);
//                 return;
//             }

//             struct ClusterBuffer cl2 = {0};

//             int8_t retcode2;

//             dest_cluster = ROOT_CLUSTER_NUMBER;

//             struct FAT32DriverRequest request2 = {
//                 .buf = &cl2,
//                 .parent_cluster_number = dest_cluster,
//                 .buffer_size = 0,
//                 .cluster_move = source_cluster,
//             };

//             memcpy(request.nama_move, source_name, strlen(source_name));
//             memcpy(request.name, dest_name, strlen(dest_name));

//             struct FAT32DirectoryTable dir_table2 = {0};

//             struct FAT32DirectoryEntry empty_entry2 = {0};

//             put(source_name, WHITE);
//             put(" ke ", WHITE);
//             put(dest_name, WHITE);
//             put("\n", WHITE);

//             for (uint32_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
//             {
//                     dir_table2.table[i] = empty_entry2;
//             }

//             request2.buf = &dir_table2;

//             syscall(1, (uint32_t)&request2, (uint32_t)&retcode2, 0); 

//             if(retcode2 == 0){
//                 put("\nPemindahan Berhasil Nyan\n\n", WHITE);
//                 put(dir_table2.table[0].name, WHITE);
//                 put("\n\n", WHITE);
//                 return;
//             }

//             else if(retcode2 == 1){
//                 put("\nGomen, Di Folder Tujuan Udah Ada Yang Namanya Sama\n\n", WHITE);
//                 put(dir_table2.table[0].name, WHITE);
//                 put("\n\n", WHITE);
//                 return;
//             }

//             else if(retcode2 == 2){
//                 put("\nGomen, Folder Tujuan Sudah Penuh\n\n", WHITE);
//             }

//             else{
//                 put("\nGomen, Tujuan Bukan Folder\n\n", WHITE);
//             }
//         }
//     }
// }

// #include "cp.h"
// #include "../header/stdlib/string.h"

// void cp(char * dir_name_source, char * dest_name){

//     struct ClusterBuffer cl = {0};

//     uint32_t cur_dir = ROOT_CLUSTER_NUMBER;

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

//     syscall(1, (uint32_t)&request, (uint32_t)&retcode, 0);

//     if(retcode == 0){

//         if(memcmp(dir_name_source, "..", 2) == 0){

//             uint32_t new_dir =  dir_table.table[1].cluster_high << 16 | dir_table.table[1].cluster_low;

//             current_working_directory_stat = new_dir;

//             for(int j = 0; j < 8; j++){

//                 current_working_directory_name_stat[j] = dir_table.table[1].name[j];

//             }

//             put("\nBerhasil Berpindah ke Directory Bapak\n\n", WHITE);

//         }

//         else
        
//         {
//             for(int i = 0; i < 64; i++){
        
//                 if(memcmp(dir_name_source, dir_table.table[i].name, strlen(dir_name_source)) == 0){
                        
//                     if (dir_table.table[i].attribute != ATTR_SUBDIRECTORY){
//                         put(dir_name_source, WHITE);
//                         put(" Bukan Directory Sir\n", WHITE);
//                         return;
//                     }

//                     else{

//                         cur_dir = dir_table.table[i].cluster_high << 16 | dir_table.table[i].cluster_low;

//                         // current_working_directory_stat = cur_dir;

//                         // for(int j = 0; j < 8; j++){
//                         //     current_working_directory_name_stat[j] = dir_table.table[i].name[j];
//                         // }

//                         put("\nBerhasil masuk ke directory ", WHITE);
//                         put(dir_name_source, WHITE);
//                         put("\n\n", WHITE);

//                         return;   

//                     }

//                 }
//             }

//             for(int i = 0; i < 64; i++){
        
//                 if(memcmp(dest_name, dir_table.table[i].name, strlen(dest_name)) == 0){
                        

//                         cur_dir = dir_table.table[i].cluster_high << 16 | dir_table.table[i].cluster_low;

//                         // current_working_directory_stat = cur_dir;

//                         // for(int j = 0; j < 8; j++){
//                         //     current_working_directory_name_stat[j] = dir_table.table[i].name[j];
//                         // }

//                         put("\nBerhasil masuk ke directory ", WHITE);
//                         put(dest_name, WHITE);
//                         put("\n\n", WHITE);

//                         return;   
//                 }
//             }

//     struct FAT32DriverRequest requestx = {
//         .buf = &cl,
//         .parent_cluster_number = cur_dir,
//         .buffer_size = 0,
//     };

//     memcpy(requestx.name, dest_name, 8);

//     struct FAT32DirectoryTable dir_tablex = {0};

//     struct FAT32DirectoryEntry empty_entryx = {0};

//     for (uint32_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
//     {
//             dir_tablex.table[i] = empty_entryx;
//     }

//     requestx.buf = &dir_tablex;

//     syscall(1, (uint32_t)&requestx, (uint32_t)&retcode, 0);
    
//     put(dir_table.table[0].name, WHITE);

//     }



//     }
//     else if(retcode == 1){
//         put("\nItu Bukan Directory\n\n", WHITE);
//     }
//     else{
//         put("\nAda Masalah Di Directory\n\n", WHITE);
//     }
// }
