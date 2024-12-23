#ifndef SCOOTERFS_H
#define SCOOTERFS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../include/fs_common.h"

#define SCOOTERFS_MAGIC         0x5C007E25
#define SCOOTERFS_BLOCK_SIZE    4096
#define SCOOTERFS_MAX_FILES     64

typedef struct {
    char name[SCOOTERFS_MAX_FILENAME];
    uint32_t inode_number;
    uint8_t type;
    uint32_t size;
    uint64_t created_time;
    uint64_t modified_time;
    bool is_deleted;
} scooterfs_dirent;

typedef struct {
    uint32_t magic;
    uint32_t total_blocks;
    uint32_t free_blocks;
    uint32_t root_inode;
    uint8_t* block_bitmap;
    scooterfs_dirent files[SCOOTERFS_MAX_FILES];
    size_t file_count;
} scooterfs_superblock;

void scooterfs_init(void);
bool scooterfs_format(void);
bool scooterfs_create_file(const char* name, bool is_directory);
bool scooterfs_delete_file(const char* name);
bool scooterfs_list_directory(scooterfs_dirent* entries, size_t* count);
uint64_t scooterfs_get_time(void);
char* scooterfs_read_file(const char* filename);
bool scooterfs_write_file(const char* filename, const char* data, size_t length);

#endif 