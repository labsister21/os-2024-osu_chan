#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "header/stdlib/string.h"
#include "header/filesystem/fat32.h"

// #define DIRECTORY_TABLE_SIZE CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry)

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

struct FAT32DriverState driver_state;

/* -- Driver Interfaces -- */

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

// ASUMSI INI BUAT cluster_map[3], BUAT ROOT dari segala ROOT, ROOT yang sesungguhnya
void init_directory_table(struct FAT32DirectoryTable *dir_table, char *name, uint32_t parent_dir_cluster)
{
    // ngasih nama directory sesuai parameter
    memcpy(dir_table->table[0].name, name, 8);

    // asumsinya nomor cluster parent sama anak itu somehow sama, atau ROOT_CLUSTER_NUMBER??
    dir_table->table[0].cluster_high = parent_dir_cluster & 0xFFFF;
    dir_table->table[0].cluster_low = (parent_dir_cluster >> 16) & 0xFFFF;

    dir_table->table[0].user_attribute = UATTR_NOT_EMPTY;
    dir_table->table[0].attribute = ATTR_SUBDIRECTORY;

    dir_table->table[0].filesize = 0;

    // entry 1 pending dulu, apalah OS ni, bingung ngisi entry 1 nya,

    //   memcpy(dir_table->table[1].name, "..      ", 8); // Nama ".." harus diatur secara eksplisit
    //     dir_table->table[1].attribute = ATTR_SUBDIRECTORY;
    //     dir_table->table[1].user_attribute = UATTR_NOT_EMPTY;
    //     dir_table->table[1].filesize = 0;
    //     dir_table->table[1].cluster_low = (ROOT_CLUSTER_NUMBER) & 0xFFFF; // Mengacu pada direktori root
    //     dir_table->table[1].cluster_high = (ROOT_CLUSTER_NUMBER >> 16) & 0xFFFF;
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
    if (memcmp(block.buf, fs_signature, BLOCK_SIZE) == 0)
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

    driver_state.fat_table.cluster_map[0] = CLUSTER_0_VALUE;
    driver_state.fat_table.cluster_map[1] = CLUSTER_1_VALUE;
    driver_state.fat_table.cluster_map[2] = FAT32_FAT_END_OF_FILE;

    // kita nulis fat table ke cluster 1, kita nulis satu cluster aja
    // kenapa cluster 1, hmmmm, ngikutin petunjuk sih , aghhhh
    write_clusters(driver_state.fat_table.cluster_map, 1, 1);

    // mari kita setting cluster 2 jadi root directory
    struct FAT32DirectoryTable temp;

    init_directory_table(&temp, "root", ROOT_CLUSTER_NUMBER);

    write_clusters(temp.table, 2, 1);
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
    read_clusters(driver_state.dir_table_buf.table, request.parent_cluster_number, 1);

    // // Cek apakah paling nggak ada 1 entry yang valid
    // bool is_empty_directory = true;
    // for (int i = 0; i < (int)(sizeof(driver_state.dir_table_buf.table) / sizeof(struct FAT32DirectoryEntry)); i++)
    // {
    //     if (driver_state.dir_table_buf.table[i].user_attribute != UATTR_NOT_EMPTY)
    //     {
    //         is_empty_directory = false;
    //         break;
    //     }
    // }

    // if (is_empty_directory)
    // {
    //     return -1; // gak ada 1 entry pun yang memenuhi syarat
    // }

    // agak ragu tapi asumsinya yang di cari adalah folder yang ada di lokasi parent_cluster-number

    if (driver_state.dir_table_buf.table[0].user_attribute == UATTR_NOT_EMPTY && driver_state.dir_table_buf.table[0].attribute == ATTR_SUBDIRECTORY)
    {

        for (int i = 0; i < (int)(sizeof(driver_state.dir_table_buf.table) / sizeof(struct FAT32DirectoryEntry)); i++)
        {
            // kalau namanya sama
            if (memcmp(driver_state.dir_table_buf.table[i].name, request.name, 8) == 0)
            {
                if (driver_state.dir_table_buf.table[i].attribute == ATTR_SUBDIRECTORY)
                {
                    // Baca direktori
                    uint32_t cluster = ((((uint32_t)(driver_state.dir_table_buf.table[i].cluster_high)) << 16) & 0xFFFF0000) | (((uint32_t)(driver_state.dir_table_buf.table[i].cluster_low)) & 0x0000FFFF);
                    read_clusters(request.buf, cluster, 1);
                    return 0; // dapet
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
    if (driver_state.dir_table_buf.table[0].user_attribute == UATTR_NOT_EMPTY)
    {

        read_clusters(driver_state.fat_table.cluster_map, FAT_CLUSTER_NUMBER, 1);

        // bool is_directory = false;
        for (int i = 0; (int)(sizeof(driver_state.dir_table_buf.table) / sizeof(struct FAT32DirectoryEntry)); i++)
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
                    if (driver_state.dir_table_buf.table[i].filesize > request.buf)
                    {
                        return 2;
                    }
                    else
                    {

                        // kalau ketemu, tinggal ngelakuin iterasi ke FAT table
                        int pengali_clus = 0;

                        // lokasi cluster pertama
                        uint32_t cluster = ((((uint32_t)(driver_state.dir_table_buf.table[i].cluster_high)) << 16) & 0xFFFF0000) | (((uint32_t)(driver_state.dir_table_buf.table[i].cluster_low)) & 0x0000FFFF);

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

/**
 * FAT32 write, write a file or folder to file system.
 *
 * @param request All attribute will be used for write, buffer_size == 0 then create a folder / directory
 * @return Error code: 0 success - 1 file/folder already exist - 2 invalid parent cluster - -1 unknown
 */
int8_t write(struct FAT32DriverRequest request)
{
}

/**
 * FAT32 delete, delete a file or empty directory (only 1 DirectoryEntry) in file system.
 *
 * @param request buf and buffer_size is unused
 * @return Error code: 0 success - 1 not found - 2 folder is not empty - -1 unknown
 */
int8_t delete(struct FAT32DriverRequest request)
{

    // baca dulu directory table dari disk terus ditaro di struct directory table yang kita buat
    read_clusters(driver_state.dir_table_buf.table, request.parent_cluster_number, 1);

    // jujur agak ragu, asumsinya root directory yang emang gak boleh dihapus itu untuk semua kasus root directory lah ya
    // bukan root directory yang paling awal aja constraint tadi berlaku
    // kalau root directory dir_table tidak kosong  dan bukan file

    int index_found = -1;
    bool cek = false;
    uint32_t cluster;
    if (driver_state.dir_table_buf.table[0].user_attribute == UATTR_NOT_EMPTY && driver_state.dir_table_buf.table[0].attribute == ATTR_SUBDIRECTORY)
    {

        // kalau root directory gak boleh dihapus, maka kita harus nge loop mulai dar 1 huh
        for (int i = 1; i < (int)(sizeof(driver_state.dir_table_buf.table) / sizeof(struct FAT32DirectoryEntry)); i++)
        {
            // kalau ketemu nama dan ekstensinya sama
            if (
                (memcmp(driver_state.dir_table_buf.table[i].name, request.name, 8) == 0) && (memcmp(driver_state.dir_table_buf.table[0].ext, request.ext, 3) == 0)
                )
            {

                // simpen informasi clusternya
                cluster = (((uint32_t)driver_state.dir_table_buf.table[i].cluster_high) << 16) | ((uint32_t)driver_state.dir_table_buf.table[i].cluster_low);

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
                struct FAT32DirectoryTable temp_loader;
                read_clusters(temp_loader.table, cluster, 1);

                bool is_empty = true;

                // ngecek kalau directory ini kosong atau gak
                for (int i = 1; i < (int)(sizeof(temp_loader.table) / sizeof(struct FAT32DirectoryEntry)); i++)
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
                        driver_state.dir_table_buf.table[index_found].name[i] = 0;
                    }

                    for (int j = 0; j < 3; j++)
                    {
                        driver_state.dir_table_buf.table[index_found].ext[j] = 0;
                    }

                    driver_state.dir_table_buf.table[index_found].user_attribute = FAT32_FAT_EMPTY_ENTRY;
                    driver_state.dir_table_buf.table[index_found].attribute = FAT32_FAT_EMPTY_ENTRY;

                    // update disk, dengan mengoverwrite dengan data yang baru setelah penghapusan
                    write_clusters(driver_state.dir_table_buf.table, request.parent_cluster_number, 1);

                    for (int i = 0; i < 8; i++)
                    {
                        temp_loader.table[0].name[i] = 0;
                    }

                    for (int j = 0; j < 3; j++)
                    {
                        temp_loader.table[0].ext[j] = 0;
                    }

                    temp_loader.table[0].attribute = FAT32_FAT_EMPTY_ENTRY;
                    temp_loader.table[0].user_attribute = FAT32_FAT_EMPTY_ENTRY;

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
                driver_state.dir_table_buf.table[index_found].attribute = FAT32_FAT_EMPTY_ENTRY;
                driver_state.dir_table_buf.table[index_found].user_attribute = FAT32_FAT_EMPTY_ENTRY;

                for (int i = 0; i < 8; i++)
                {
                    driver_state.dir_table_buf.table[index_found].name[i] = 0;
                }

                for (int i = 0; i < 3; i++)
                {
                    driver_state.dir_table_buf.table[index_found].ext[i] = 0;
                }

                write_cluster(driver_state.dir_table_buf.table, request.parent_cluster_number, 1);

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