#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "header/stdlib/string.h"
#include "header/filesystem/fat32.h"

#define MAX_FOR_CLUSTER_FAT_HEX 0x800

const uint8_t fs_signature[BLOCK_SIZE] = {
    'C',
    'o',
    'u',
    'r',
    's',
    'e',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    'D',
    'e',
    's',
    'i',
    'g',
    'n',
    'e',
    'd',
    ' ',
    'b',
    'y',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    'L',
    'a',
    'b',
    ' ',
    'S',
    'i',
    's',
    't',
    'e',
    'r',
    ' ',
    'I',
    'T',
    'B',
    ' ',
    ' ',
    'M',
    'a',
    'd',
    'e',
    ' ',
    'w',
    'i',
    't',
    'h',
    ' ',
    '<',
    '3',
    ' ',
    ' ',
    ' ',
    ' ',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '2',
    '0',
    '2',
    '4',
    '\n',
    [BLOCK_SIZE - 2] = 'O',
    [BLOCK_SIZE - 1] = 'k',
};

struct FAT32DriverState driver_state = {0};

/**
 * Convert cluster number to logical block address
 *
 * @param cluster Cluster number to convert
 * @return uint32_t Logical Block Address
 */
uint32_t cluster_to_lba(uint32_t cluster)
{
    return cluster * CLUSTER_BLOCK_COUNT;
}

/**
 * Initialize DirectoryTable value with
 * - Entry-0: DirectoryEntry about itself
 * - Entry-1: Parent DirectoryEntry
 *
 * @param dir_table          Pointer to directory table
 * @param name               8-byte char for directory name
 * @param parent_dir_cluster Parent directory cluster number
 */

// Fungsi ini agak sus, kita tau cluster dari direktori dia sendiri itu dari mana cik?
// kalau misalkan parent_dir_cluster itu info cluster buat bapak
// kalau misalkan parent_dir_cluster itu buat anak
// otomatis fungsi ini cuman buat root yang awal banget yang di cluster_map[3]??
// soale cluster anak dan bapak jelas jelas bisa beda, selain root
// Apalah OS ni

// Ini dipake buat root aja lah wkwkwk, bingung cik
void init_directory_table(struct FAT32DirectoryTable *dir_table, char *name, uint32_t parent_dir_cluster)
{
    // ngosongin buat ngilangin giberrish
    struct FAT32DirectoryEntry empty_entry = {0};
    for (uint32_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
        dir_table->table[i] = empty_entry;
    }

    memcpy(dir_table->table[0].name, name, 8);


    dir_table->table[1].cluster_high = (uint16_t)(parent_dir_cluster >> 16);
    dir_table->table[1].cluster_low = (uint16_t)(parent_dir_cluster & 0xFFFF);
}

/**
 * Checking whether filesystem signature is missing or not in boot sector
 *
 * @return True if memcmp(boot_sector, fs_signature) returning inequality
 */
bool is_empty_storage(void)
{
    struct BlockBuffer block;
    read_blocks(block.buf, BOOT_SECTOR, 1);
    if (memcmp(block.buf, fs_signature, BLOCK_SIZE) != 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * Create new FAT32 file system. Will write fs_signature into boot sector and
 * proper FileAllocationTable (contain CLUSTER_0_VALUE, CLUSTER_1_VALUE,
 * and initialized root directory) into cluster number 1
 */
void create_fat32(void)
{
    write_blocks(fs_signature, BOOT_SECTOR, 1);


    // kita nulis fat table ke cluster 1, kita nulis satu cluster aja
    // kenapa cluster 1, hmmmm, ngikutin petunjuk sih , aghhhh
    write_clusters(driver_state.fat_table.cluster_map, 1, 1);

    // mari kita setting cluster 2 jadi root directory
    struct FAT32DirectoryTable temp;

    init_directory_table(&temp, "root\0\0\0\0", ROOT_CLUSTER_NUMBER);

    driver_state.fat_table.cluster_map[0] = CLUSTER_0_VALUE;
    driver_state.fat_table.cluster_map[1] = CLUSTER_1_VALUE;
    driver_state.fat_table.cluster_map[2] = FAT32_FAT_END_OF_FILE;

    temp.table[0].user_attribute = UATTR_NOT_EMPTY;
    temp.table[0].attribute = ATTR_SUBDIRECTORY;
    temp.table[0].filesize = 0;
    
    temp.table[0].cluster_high = 0;
    temp.table[0].cluster_low = 2;

    memcpy(temp.table[1].name, "root\0\0\0\0", 8);

    temp.table[1].attribute = ATTR_SUBDIRECTORY;

    temp.table[1].user_attribute = UATTR_NOT_EMPTY;

    write_clusters(&temp, 2, 1);
}

/**
 * Initialize file system driver state, if is_empty_storage() then create_fat32()
 * Else, read and cache entire FileAllocationTable (located at cluster number 1) into driver state
 */
void initialize_filesystem_fat32(void)
{
    if (is_empty_storage())
    {
        create_fat32();
    }
    else
    {

        read_clusters(&driver_state.fat_table, 1, 1);
    }
}

/**
 * Write cluster operation, wrapper for write_blocks().
 * Recommended to use struct ClusterBuffer
 *
 * @param ptr            Pointer to source data
 * @param cluster_number Cluster number to write
 * @param cluster_count  Cluster count to write, due limitation of write_blocks block_count 255 => max cluster_count = 63
 */
void write_clusters(const void *ptr, uint32_t cluster_number, uint8_t cluster_count)
{
    write_blocks(ptr, cluster_to_lba(cluster_number), CLUSTER_BLOCK_COUNT * cluster_count);
}

/**
 * Read cluster operation, wrapper for read_blocks().
 * Recommended to use struct ClusterBuffer
 *
 * @param ptr            Pointer to buffer for reading
 * @param cluster_number Cluster number to read
 * @param cluster_count  Cluster count to read, due limitation of read_blocks block_count 255 => max cluster_count = 63
 */
void read_clusters(void *ptr, uint32_t cluster_number, uint8_t cluster_count)
{
    read_blocks(ptr, cluster_to_lba(cluster_number), CLUSTER_BLOCK_COUNT * cluster_count);
}

/* -- CRUD Operation -- */

/**
 *  FAT32 Folder / Directory read
 *
 * @param request buf point to struct FAT32DirectoryTable,
 *                name is directory name,
 *                ext is unused,
 *                parent_cluster_number is target directory table to read,
 *                buffer_size must be exactly sizeof(struct FAT32DirectoryTable)
 * @return Error code: 0 success - 1 not a folder - 2 not found - -1 unknown
 */
int8_t read_directory(struct FAT32DriverRequest request)
{
    read_clusters(&driver_state.dir_table_buf, request.parent_cluster_number, 1);

    if (driver_state.dir_table_buf.table[0].attribute == ATTR_SUBDIRECTORY)
    {

        for (int i = 0; i < (int)(CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry)); i++)
        {
            // kalau namanya sama
            if (memcmp(driver_state.dir_table_buf.table[i].name, request.name, strlen(request.name)) == 0)
            {
                if (driver_state.dir_table_buf.table[i].attribute == ATTR_SUBDIRECTORY)
                {
                    uint32_t dir_cluster_number = (driver_state.dir_table_buf.table[i].cluster_high << 16) | driver_state.dir_table_buf.table[i].cluster_low;

                    read_clusters(request.buf, dir_cluster_number, 1);

                    return 0; 
                }
                else
                {
                    return 1; // bukan directory
                }
            }
        }
        // kalau udah ngecekin directory entry tapi gak ada, yaudah, gak dapet
        return 2;
    }

    return -1;
}

/**
 * FAT32 read, read a file from file system.
 *
 * @param request All attribute will be used for read, buffer_size will limit reading count
 * @return Error code: 0 success - 1 not a file - 2 not enough buffer - 3 not found - -1 unknown
 */
int8_t read(struct FAT32DriverRequest request)
{
    read_clusters(driver_state.dir_table_buf.table, request.parent_cluster_number, 1);

    // kalau entry root gak valid yaudah
    if (driver_state.dir_table_buf.table[0].user_attribute == UATTR_NOT_EMPTY && driver_state.dir_table_buf.table[0].attribute == ATTR_SUBDIRECTORY )
    {

        read_clusters(driver_state.fat_table.cluster_map, FAT_CLUSTER_NUMBER, 1);

        // bool is_directory = false;
        for (int i = 0;(int)(CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry)); i++)
        {

            // kalau misalkan nama file + ekstensinya cocok, hmm tapi gimana kalau file binary, kayak chall??? ini pikirin nanti lah ya
            if ((memcmp(driver_state.dir_table_buf.table[i].name, request.name, 8) == 0) && (memcmp(driver_state.dir_table_buf.table[i].ext, request.ext, 3) == 0))
            {

                // kalau ternyata dia directory
                if (driver_state.dir_table_buf.table[i].attribute == ATTR_SUBDIRECTORY)
                {
                    return 1;
                }
                else
                {
                    // kalau ukuran dari file nya lebih gedhe dari buffer
                    if (driver_state.dir_table_buf.table[i].filesize > request.buffer_size)
                    {
                        return 2;
                    }
                    else
                    {

                        // kalau ketemu, tinggal ngelakuin iterasi ke FAT table
                        int pengali_clus = 0;

                        // lokasi cluster pertama
                        uint32_t cluster = driver_state.dir_table_buf.table[i].cluster_high << 16 | driver_state.dir_table_buf.table[i].cluster_low;

                        while (cluster != FAT32_FAT_END_OF_FILE)
                        {
                            // request.buf + cluster_size * pengali itu buat nyari offset untuk setiap kita mau naro cluster di buffer, biar ga ke overwrite
                            read_clusters(request.buf + CLUSTER_SIZE * pengali_clus, cluster, 1);

                            // baca lokasi cluster berikutnya
                            cluster = driver_state.fat_table.cluster_map[cluster];

                            pengali_clus += 1;
                        }

                        return 0;
                    }
                }
            }
        }

        // kalau udah ngelakuin iterasi ke directory table tapi gak ketemu, yaudah
        return 3;
    }

    return -1;
}

int8_t read_file(struct FAT32DriverRequest request)
{   
    
    read_clusters(driver_state.dir_table_buf.table, request.parent_cluster_number, 1);

    // kalau entry root gak valid yaudah
    if (driver_state.dir_table_buf.table[0].user_attribute == UATTR_NOT_EMPTY && driver_state.dir_table_buf.table[0].attribute == ATTR_SUBDIRECTORY )
    {

        read_clusters(&driver_state.fat_table, FAT_CLUSTER_NUMBER, 1);

        char temp_nama[10];

        clear(temp_nama, 10);

        char temp_ext[10];

        clear(temp_ext, 10);

        // temp_nama[0] = 't';
        // temp_nama[1] = 'r';
        // temp_nama[2] = 'o';
        // temp_nama[3] = 'j';
        // temp_nama[4] = 'a';
        // temp_nama[5] = 'n';


        // // bool is_directory = false;
        for (int i = 2;(int)(CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry)); i++)
        {
            memcpy(temp_nama, request.name, 8);
            memcpy(temp_ext, request.ext, 3);

            if ((areStringsEqual(driver_state.dir_table_buf.table[i].name, temp_nama, strlen(temp_nama)) == 1))
            {
                if((areStringsEqual(driver_state.dir_table_buf.table[i].ext, temp_ext, strlen(temp_ext)) == 1)){
                    
                

        //         // kalau ternyata dia directory
                if (driver_state.dir_table_buf.table[i].attribute == ATTR_SUBDIRECTORY)
                {
                    return 1;
                }
                else
                {
        //             // kalau ukuran dari file nya lebih gedhe dari buffer
                    if (driver_state.dir_table_buf.table[i].filesize > request.buffer_size)
                    {
                        return 2;
                    }
                    else
                    {

                        // kalau ketemu, tinggal ngelakuin iterasi ke FAT table
                        int pengali_clus = 0;

                        // lokasi cluster pertama
                        uint32_t cluster = driver_state.dir_table_buf.table[i].cluster_high << 16 | driver_state.dir_table_buf.table[i].cluster_low;

                        while (cluster != FAT32_FAT_END_OF_FILE)
                        {
                            // request.buf + cluster_size * pengali itu buat nyari offset untuk setiap kita mau naro cluster di buffer, biar ga ke overwrite
                            read_clusters(request.buf + CLUSTER_SIZE * pengali_clus, cluster, 1);

                            // baca lokasi cluster berikutnya
                            cluster = driver_state.fat_table.cluster_map[cluster];

                            pengali_clus += 1;
                        }

                        return 0;
 
                       }
                    }
          
                }
            }

            clear(temp_nama, 10);
            clear(temp_ext, 10);

        // // kalau udah ngelakuin iterasi ke directory table tapi gak ketemu, yaudah
        }

        return 3;
    }

    return -1;
}


/**
 * FAT32 write, write a file or folder to file system.
 *
 * @param request All attribute will be used for write, buffer_size == 0 then create a folder / directory
 * @return Error code: 0 success - 1 file/folder already exist - 2 invalid parent cluster - -1 unknown
 */
int8_t write(struct FAT32DriverRequest request)
{
    // baca directory table dari disk, terus ditaro di dir_table_buf.table
    read_clusters(&driver_state.dir_table_buf, request.parent_cluster_number, 1);

    if (driver_state.dir_table_buf.table[0].attribute == ATTR_SUBDIRECTORY)
    {

        bool is_full = true;
        
        int index_found = 0;

        for (int i = 0; i < (int)(CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry)); i++)
        {

            if (driver_state.dir_table_buf.table[i].user_attribute != UATTR_NOT_EMPTY)
            {
                // berarti belum penuh
                index_found = i;
                is_full = false;
                break;
            }
        }

        // kalau udah penuh
        if (is_full == true)
        {
            return -1;
        }

        bool cek_nama = false;

        for (int i = 1; i < (int)(CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry)); i++)
        {
            if ((memcmp(driver_state.dir_table_buf.table[i].name, request.name, 8) == 0) && (memcmp(driver_state.dir_table_buf.table[i].ext, request.ext, 3) == 0))
            {
                cek_nama = true;

                break;
            }
        }

        // kalau file/folder udah ada, asumsinya ngecek dari nama dan ekstensi aja itu udah cukup
        if (cek_nama == true)
        {
            return 1;
        }

        // sesuai petunjuk kalau buffer_size == 0, maka  bikin directory / sub directory
        if (request.buffer_size == 0)
        {
            uint32_t empty_entry = 0;

            bool is_not_empty = true;

            for (uint32_t hex_cluster = 0; hex_cluster < MAX_FOR_CLUSTER_FAT_HEX; hex_cluster++)
            {
                if (driver_state.fat_table.cluster_map[hex_cluster] == FAT32_FAT_EMPTY_ENTRY)
                {
                    empty_entry = hex_cluster;

                    is_not_empty = false;

                    break;
                }
            }

            // gak ada entry kosong yang bisa di pake di file Allocation Table
            if (is_not_empty)
            {
                return -1;
            }

            // entry yang udah didapet kita kasih tanda end_of_file karena dia directory
            driver_state.fat_table.cluster_map[empty_entry] = FAT32_FAT_END_OF_FILE;

            // tulis ke disk
            write_clusters(driver_state.fat_table.cluster_map, FAT_CLUSTER_NUMBER, 1);

            // kita update parent directory
            for (int i = 0; i < 8; i++)
            {
                driver_state.dir_table_buf.table[index_found].name[i] = request.name[i];
            }

            for (int i = 0; i < 3; i++)
            {
                driver_state.dir_table_buf.table[index_found].ext[i] = request.ext[i];
            }

            driver_state.dir_table_buf.table[index_found].attribute = ATTR_SUBDIRECTORY;
            driver_state.dir_table_buf.table[index_found].user_attribute = UATTR_NOT_EMPTY;
            driver_state.dir_table_buf.table[index_found].cluster_high = (uint16_t)(empty_entry >> 16);
            driver_state.dir_table_buf.table[index_found].cluster_low = (uint16_t)(empty_entry & 0xFFFF);
            driver_state.dir_table_buf.table[index_found].filesize = request.buffer_size;

            // kita update disk dengan nilai yang sudah diperbaharui
            write_clusters(driver_state.dir_table_buf.table, request.parent_cluster_number, 1);

            // kita perlu bikin directory tabel baru buat directory baru????
            // update 24/04/2024, keknya emang bikin directory table baru, harusnya

            // ini harusnya dah bener, yang di load ke struct itu cluster dari empty_entry, tempat kita naro sebelumnya
            // tapi behaviornya masih aneh
            read_clusters(&driver_state.dir_table_buf, empty_entry, 1);

            driver_state.dir_table_buf.table[0].cluster_high = (uint16_t) empty_entry >> 16;
            driver_state.dir_table_buf.table[0].cluster_low = (uint16_t) empty_entry;

            driver_state.dir_table_buf.table[1].cluster_high = (uint16_t) (request.parent_cluster_number >> 16);
            driver_state.dir_table_buf.table[1].cluster_low =  (uint16_t) (request.parent_cluster_number & 0xFFFF);

            driver_state.dir_table_buf.table[1].user_attribute = UATTR_NOT_EMPTY;
            driver_state.dir_table_buf.table[1].attribute = ATTR_SUBDIRECTORY;

            memcpy(driver_state.dir_table_buf.table[1].name, request.nama_bapak, 8);

            // kita setting buat table[0] itu ada informasi dirinya sendiri
            for (int i = 0; i < 8; i++)
            {
                driver_state.dir_table_buf.table[0].name[i] = request.name[i];
            }

            for (int i = 0; i < 3; i++)
            {
                driver_state.dir_table_buf.table[0].ext[i] = request.ext[i];
            }

            driver_state.dir_table_buf.table[0].attribute = ATTR_SUBDIRECTORY;
            driver_state.dir_table_buf.table[0].user_attribute = UATTR_NOT_EMPTY;

            driver_state.dir_table_buf.table[0].filesize = request.buffer_size; // ini sebenernya otomatis 0

            // tulis lagi ke tempat tadi
            write_clusters(driver_state.dir_table_buf.table, empty_entry, 1);

        }
        else
        {
            // banyak partisi yang diperluin
            // kalau misalkan di modulo masih nyisa, dibuletin ke atas
            int count_cluster;
            if ((request.buffer_size % CLUSTER_SIZE) != 0)
            {
                count_cluster = request.buffer_size / CLUSTER_SIZE;
                count_cluster += 1;
            }
            else
            {
                count_cluster = request.buffer_size / CLUSTER_SIZE;
            }

            bool is_exist = false;
            int cluster_ready = 0;
            uint32_t start = 0x0;
            // cari tahu dulu, entry FAT table yang bisa kita pake itu mulai dari mana
            for (uint32_t i = 0x0; i < MAX_FOR_CLUSTER_FAT_HEX; i++)
            {
                if (driver_state.fat_table.cluster_map[i] == FAT32_FAT_EMPTY_ENTRY)
                {
                    cluster_ready = 1;
                    is_exist = true;
                    start = i;
                    break;
                }
            }

            // aman
            if (is_exist == false)
            {
                return -1;
            }

            // melakukan pemetaan FAT table atau cluster_map, sama ngecek jumlah yang tersedia
            uint32_t prev = start;
            uint32_t now = start + 1;

            for (int i = now; i < MAX_FOR_CLUSTER_FAT_HEX; i++)
            {
                if (driver_state.fat_table.cluster_map[i] == FAT32_FAT_EMPTY_ENTRY)
                {
                    driver_state.fat_table.cluster_map[prev] = i;
                    prev = i;
                    cluster_ready += 1;
                }

                // kalau misalkan  cluster yang dibutuhkan udah lebih dari sama dengan cukup
                if (cluster_ready >= count_cluster)
                {
                    break;
                }
            }

            if (cluster_ready >= count_cluster)
            {
                // partisi paling terakhir jadi end_of_file
                driver_state.fat_table.cluster_map[prev] = FAT32_FAT_END_OF_FILE;

                // perbarui data FAT table ke disk
                write_clusters(driver_state.fat_table.cluster_map, FAT_CLUSTER_NUMBER, 1);

                // karena preparation sudah complete, terimakasih atas segala sarannya, waktunya mengupdate directory table sesuai request
                for (int i = 0; i < 8; i++)
                {
                    driver_state.dir_table_buf.table[index_found].name[i] = request.name[i];
                }

                for (int i = 0; i < 3; i++)
                {
                    driver_state.dir_table_buf.table[index_found].ext[i] = request.ext[i];
                }

                driver_state.dir_table_buf.table[index_found].user_attribute = UATTR_NOT_EMPTY;

                driver_state.dir_table_buf.table[index_found].cluster_high = (uint16_t) start >> 16;
                driver_state.dir_table_buf.table[index_found].cluster_low = (uint16_t) start;

                driver_state.dir_table_buf.table[index_found].filesize = request.buffer_size;

                // kita update disk dengan nilai yang sudah diperbaharui
                write_clusters(driver_state.dir_table_buf.table, request.parent_cluster_number, 1);

                // tinggal update file ke disk
                int i = 0;
                while (driver_state.fat_table.cluster_map[start] != FAT32_FAT_END_OF_FILE)
                {
                    write_clusters((uint8_t *)request.buf + CLUSTER_SIZE * i, start, 1);
                    start = driver_state.fat_table.cluster_map[start];
                    i++;
                }
                write_clusters((uint8_t *)request.buf + CLUSTER_SIZE * i, start, 1);
            }
            else
            {
                // kalau misalkan banyak cluster yang dibutuhin gak cukup, kosongin lagi yang udah terlanjur di loop sebelumnya
                uint32_t prev_2;
                while (driver_state.fat_table.cluster_map[start] != FAT32_FAT_EMPTY_ENTRY)
                {
                    prev_2 = driver_state.fat_table.cluster_map[start];
                    driver_state.fat_table.cluster_map[start] = FAT32_FAT_EMPTY_ENTRY;
                    start = prev_2;
                }
                return -1;
            }
        }

        return 0;
    }

    return 2;
}

/**
 * FAT32 delete, delete a file or empty directory (only 1 DirectoryEntry) in file system.
 *
 * @param request buf and buffer_size is unused
 * @return Error code: 0 success - 1 not found - 2 folder is not empty - -1 unknown
 */
int8_t delete_something(struct FAT32DriverRequest request)
{

    // baca dulu directory table dari disk terus ditaro di struct directory table yang kita buat
    read_clusters(&driver_state.dir_table_buf, request.parent_cluster_number, 1);

    // jujur agak ragu, asumsinya root directory yang emang gak boleh dihapus itu untuk semua kasus root directory lah ya
    // bukan root directory yang paling awal aja constraint tadi berlaku
    // kalau root directory dir_table tidak kosong  dan bukan file

    int index_found = -1;

    bool cek = false;

    uint32_t cluster;

    if (driver_state.dir_table_buf.table[0].attribute == ATTR_SUBDIRECTORY)
    {

        // kalau root directory gak boleh dihapus, maka kita harus nge loop mulai dar 1 huh
        for (int i = 2; i < (int)(CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry)); i++)
        {
            // kalau ketemu nama dan ekstensinya sama
            if (
                (memcmp(driver_state.dir_table_buf.table[i].name, request.name, strlen(request.name)) == 0) && (memcmp(driver_state.dir_table_buf.table[i].ext, request.ext, 3) == 0)
            )
            {

                // simpen informasi clusternya
                cluster = (driver_state.dir_table_buf.table[i].cluster_high << 16) | driver_state.dir_table_buf.table[i].cluster_low;

                cek = true;

                // simpen indeksnya
                index_found = i;

                break;
            }
        }

        if (cek == false)
        {
            return 1;
        }
        else
        {
            // hapus directory dulu kali yak
            // mastiin dulu directory kosong dengan ngeloop ke directory table
            // kalau directory kosong, kita hapus

            // kalau misalkan directory
            if (driver_state.dir_table_buf.table[index_found].attribute == ATTR_SUBDIRECTORY)
            {
                struct FAT32DirectoryEntry empty_entry = {0};
                
                struct FAT32DirectoryTable temp_loader;

                for (uint32_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
                {
                        temp_loader.table[i] = empty_entry;
                }

                read_clusters(&temp_loader, cluster, 1);

                bool is_empty = true;

                // ngecek kalau directory ini kosong atau gak
                for (int i = 2; i < (int)(CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry)); i++)
                {
                    if (temp_loader.table[i].user_attribute == UATTR_NOT_EMPTY)
                    {
                        is_empty = false;
                        break;
                    }
                }

                if (!is_empty)
                {
                    return 2;
                }
                else
                {
                    // kalau folder gak kosong, dari sini mulai dilakukan proses penghapusan

                    // hapus dari entry yang ada pada directory table utama
                    for (int i = 0; i < 8; i++)
                    {
                        driver_state.dir_table_buf.table[index_found].name[i] = '\0';
                    }

                    for (int j = 0; j < 3; j++)
                    {
                        driver_state.dir_table_buf.table[index_found].ext[j] = '\0';
                    }

                    driver_state.dir_table_buf.table[index_found].user_attribute = FAT32_FAT_EMPTY_ENTRY;
                    driver_state.dir_table_buf.table[index_found].attribute = FAT32_FAT_EMPTY_ENTRY;

                    // update disk, dengan mengoverwrite dengan data yang baru setelah penghapusan
                    write_clusters(driver_state.dir_table_buf.table, request.parent_cluster_number, 1);

                    for (int i = 0; i < 8; i++)
                    {
                        temp_loader.table[0].name[i] = '\0';
                        temp_loader.table[1].name[i] = '\0';
                    }

                    for (int j = 0; j < 3; j++)
                    {
                        temp_loader.table[0].ext[j] = '\0';
                        temp_loader.table[1].ext[j] = '\0';
                    }

                    temp_loader.table[0].attribute = FAT32_FAT_EMPTY_ENTRY;
                    temp_loader.table[0].user_attribute = FAT32_FAT_EMPTY_ENTRY;

                    temp_loader.table[1].attribute = FAT32_FAT_EMPTY_ENTRY;
                    temp_loader.table[1].user_attribute = FAT32_FAT_EMPTY_ENTRY;

                    write_clusters(temp_loader.table, cluster, 1);

                    // hapus dari cluster dari FAT
                    driver_state.fat_table.cluster_map[cluster] = FAT32_FAT_EMPTY_ENTRY;

                    // update hasil penghapusan ke disk
                    write_clusters(driver_state.fat_table.cluster_map, FAT_CLUSTER_NUMBER, 1);
                }
            }
            // kalau ternyata file yang dihapus
            else
            {
                // mirip kayak ngehapus directory tapi gak harus ngecekin directory table entry nya
                // dan harus ngehapusin entry di FAT table

                for (int i = 0; i < 8; i++)
                {
                    driver_state.dir_table_buf.table[index_found].name[i] = '\0';
                }

                for (int i = 0; i < 3; i++)
                {
                    driver_state.dir_table_buf.table[index_found].ext[i] = '\0';
                }

                driver_state.dir_table_buf.table[index_found].attribute = FAT32_FAT_EMPTY_ENTRY;
                driver_state.dir_table_buf.table[index_found].user_attribute = FAT32_FAT_EMPTY_ENTRY;

                write_clusters(driver_state.dir_table_buf.table, request.parent_cluster_number, 1);

                uint32_t prev = 0;
                
                uint32_t now = cluster;

                while (driver_state.fat_table.cluster_map[now] != FAT32_FAT_END_OF_FILE)
                {
                    prev = now;
                    now = driver_state.fat_table.cluster_map[now];
                    driver_state.fat_table.cluster_map[prev] = FAT32_FAT_EMPTY_ENTRY;
                }

                driver_state.fat_table.cluster_map[now] = 0;

                write_clusters(driver_state.fat_table.cluster_map, FAT_CLUSTER_NUMBER, 1);
            }

            // penghapusan berhasil dilakukan
            return 0;
        }
    }

    return -1;
}

// // return Error code: 0 success - 1 nama udah ada - 2 udah penuh - -1 bukan folder
int8_t update_directory_table_folder(struct FAT32DriverRequest request){


    read_clusters(&driver_state.dir_table_buf, request.parent_cluster_number, 1);


    if(driver_state.dir_table_buf.table[0].attribute != ATTR_SUBDIRECTORY){
        return -1;
    }
    
    bool is_full = true;
        
    int index_found = 0;

    for (int i = 0; i < (int)(CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry)); i++)
    {

        if (driver_state.dir_table_buf.table[i].user_attribute != UATTR_NOT_EMPTY)
        {
            // berarti belum penuh
            index_found = i;
            is_full = false;
            break;
        }
    }

    // kalau udah penuh
    if (is_full == true)
    {
        return 2;
    }

    bool cek_nama = false;

    for (int i = 2; i < (int)(CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry)); i++)
    {
        if ((memcmp(driver_state.dir_table_buf.table[i].name, request.name, 8) == 0))
        {
            cek_nama = true;

            break;
        }
    }

    // kalau file/folder udah ada, asumsinya ngecek dari nama dan ekstensi aja itu udah cukup
    if (cek_nama == true)
    {
        return 1;
    }

    for (int i = 0; i < 8; i++)
    {
        driver_state.dir_table_buf.table[index_found].name[i] = request.name[i];
    }

    for (int i = 0; i < 3; i++)
    {
        driver_state.dir_table_buf.table[index_found].ext[i] = '\0';
    }

    driver_state.dir_table_buf.table[index_found].user_attribute = UATTR_NOT_EMPTY;
    driver_state.dir_table_buf.table[index_found].attribute = ATTR_SUBDIRECTORY;

    driver_state.dir_table_buf.table[index_found].cluster_high = (uint16_t) request.cluster_move >> 16;
    driver_state.dir_table_buf.table[index_found].cluster_low = (uint16_t) request.cluster_move;

    driver_state.dir_table_buf.table[index_found].filesize = request.buffer_size;

    // kita update disk dengan nilai yang sudah diperbaharui
    write_clusters(driver_state.dir_table_buf.table, request.parent_cluster_number, 1);   

    return 0;
}


int8_t delete_folder_source(struct FAT32DriverRequest request){


    read_clusters(driver_state.dir_table_buf.table, request.parent_cluster_number, 1);

    int8_t retcode = -1;

    for (int i = 2; i < (int)(CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry)); i++)
    {
        if (memcmp(driver_state.dir_table_buf.table[i].name, request.name, 8) == 0){}
        {
            
            driver_state.dir_table_buf.table[i].attribute = FAT32_FAT_EMPTY_ENTRY;
            driver_state.dir_table_buf.table[i].user_attribute = FAT32_FAT_EMPTY_ENTRY;
            for (int k = 0; k < 8; k++)
            {
                driver_state.dir_table_buf.table[i].name[k] = '\0';
            }

            for (int l = 0; l < 3; l++)
            {
                driver_state.dir_table_buf.table[i].ext[l] = '\0';
            }

            retcode = 0;
            break;     
        }
    }

    write_clusters(driver_state.dir_table_buf.table, request.parent_cluster_number, 1);

    return retcode;
}


int8_t delete_something2(struct FAT32DriverRequest request)
{

    // baca dulu directory table dari disk terus ditaro di struct directory table yang kita buat
    read_clusters(&driver_state.dir_table_buf, request.parent_cluster_number, 1);

    // jujur agak ragu, asumsinya root directory yang emang gak boleh dihapus itu untuk semua kasus root directory lah ya
    // bukan root directory yang paling awal aja constraint tadi berlaku
    // kalau root directory dir_table tidak kosong  dan bukan file

    int index_found = -1;

    bool cek = false;

    uint32_t cluster;

    if (driver_state.dir_table_buf.table[0].attribute == ATTR_SUBDIRECTORY)
    {

        // kalau root directory gak boleh dihapus, maka kita harus nge loop mulai dar 1 huh
        for (int i = 2; i < (int)(CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry)); i++)
        {
            // kalau ketemu nama dan ekstensinya sama
            if (
                (memcmp(driver_state.dir_table_buf.table[i].name, request.name, strlen(request.name)) == 0) && (memcmp(driver_state.dir_table_buf.table[i].ext, request.ext, 3) == 0)
            )
            {

                // simpen informasi clusternya
                cluster = (driver_state.dir_table_buf.table[i].cluster_high << 16) | driver_state.dir_table_buf.table[i].cluster_low;

                cek = true;

                // simpen indeksnya
                index_found = i;

                break;
            }
        }

        if (cek == false)
        {
            return 1;
        }
        else
        {
            // hapus directory dulu kali yak
            // mastiin dulu directory kosong dengan ngeloop ke directory table
            // kalau directory kosong, kita hapus

            // kalau misalkan directory
            if (driver_state.dir_table_buf.table[index_found].attribute == ATTR_SUBDIRECTORY)
            {
                struct FAT32DirectoryEntry empty_entry = {0};
                
                struct FAT32DirectoryTable temp_loader;

                for (uint32_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
                {
                        temp_loader.table[i] = empty_entry;
                }

                read_clusters(&temp_loader, cluster, 1);

                bool is_empty = true;

                // ngecek kalau directory ini kosong atau gak
                for (int i = 2; i < (int)(CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry)); i++)
                {
                    if (temp_loader.table[i].user_attribute == UATTR_NOT_EMPTY)
                    {
                        is_empty = false;
                        break;
                    }
                }

                if (!is_empty)
                {
                    return 2;
                }
                else
                {
                    // kalau folder gak kosong, dari sini mulai dilakukan proses penghapusan

                    // hapus dari entry yang ada pada directory table utama
                    for (int i = 0; i < 8; i++)
                    {
                        driver_state.dir_table_buf.table[index_found].name[i] = '\0';
                    }

                    for (int j = 0; j < 3; j++)
                    {
                        driver_state.dir_table_buf.table[index_found].ext[j] = '\0';
                    }

                    driver_state.dir_table_buf.table[index_found].user_attribute = FAT32_FAT_EMPTY_ENTRY;
                    driver_state.dir_table_buf.table[index_found].attribute = FAT32_FAT_EMPTY_ENTRY;

                    // update disk, dengan mengoverwrite dengan data yang baru setelah penghapusan
                    write_clusters(driver_state.dir_table_buf.table, request.parent_cluster_number, 1);

                    for (int i = 0; i < 8; i++)
                    {
                        temp_loader.table[0].name[i] = '\0';
                        temp_loader.table[1].name[i] = '\0';
                    }

                    for (int j = 0; j < 3; j++)
                    {
                        temp_loader.table[0].ext[j] = '\0';
                        temp_loader.table[1].ext[j] = '\0';
                    }

                    temp_loader.table[0].attribute = FAT32_FAT_EMPTY_ENTRY;
                    temp_loader.table[0].user_attribute = FAT32_FAT_EMPTY_ENTRY;

                    temp_loader.table[1].attribute = FAT32_FAT_EMPTY_ENTRY;
                    temp_loader.table[1].user_attribute = FAT32_FAT_EMPTY_ENTRY;

                    write_clusters(temp_loader.table, cluster, 1);
                    // // hapus dari cluster dari FAT
                    // driver_state.fat_table.cluster_map[cluster] = FAT32_FAT_EMPTY_ENTRY;

                    // // update hasil penghapusan ke disk
                    // write_clusters(driver_state.fat_table.cluster_map, FAT_CLUSTER_NUMBER, 1);
                }
            }
            // kalau ternyata file yang dihapus
            else
            {
                // mirip kayak ngehapus directory tapi gak harus ngecekin directory table entry nya
                // dan harus ngehapusin entry di FAT table

                for (int i = 0; i < 8; i++)
                {
                    driver_state.dir_table_buf.table[index_found].name[i] = '\0';
                }

                for (int i = 0; i < 3; i++)
                {
                    driver_state.dir_table_buf.table[index_found].ext[i] = '\0';
                }

                driver_state.dir_table_buf.table[index_found].attribute = FAT32_FAT_EMPTY_ENTRY;
                driver_state.dir_table_buf.table[index_found].user_attribute = FAT32_FAT_EMPTY_ENTRY;

                write_clusters(driver_state.dir_table_buf.table, request.parent_cluster_number, 1);

                uint32_t prev = 0;
                
                uint32_t now = cluster;

                while (driver_state.fat_table.cluster_map[now] != FAT32_FAT_END_OF_FILE)
                {
                    prev = now;
                    now = driver_state.fat_table.cluster_map[now];
                    driver_state.fat_table.cluster_map[prev] = FAT32_FAT_EMPTY_ENTRY;
                }

                driver_state.fat_table.cluster_map[now] = 0;

                write_clusters(driver_state.fat_table.cluster_map, FAT_CLUSTER_NUMBER, 1);
            }

            // penghapusan berhasil dilakukan
            return 0;
        }
    }

    return -1;
}