#include "scooterfs.h"
#include "../memory.h"
#include "../string.h"

static scooterfs_superblock sb;
static uint8_t* fs_buffer;
static uint64_t system_time = 0;

uint64_t scooterfs_get_time(void) {
    return system_time++;
}

void scooterfs_init(void) {
    fs_buffer = kmalloc(SCOOTERFS_BLOCK_SIZE);
    sb.magic = SCOOTERFS_MAGIC;
    sb.total_blocks = 1024;
    sb.free_blocks = sb.total_blocks - 1;
    sb.root_inode = 1;
    sb.block_bitmap = kmalloc(sb.total_blocks / 8);
    sb.file_count = 0;
    
    scooterfs_create_file("/", true);
}

bool scooterfs_create_file(const char* name, bool is_directory) {
    if (sb.file_count >= SCOOTERFS_MAX_FILES) {
        return false;
    }
    
    // Checking if the file exists
    for (size_t i = 0; i < sb.file_count; i++) {
        if (!sb.files[i].is_deleted && strcmp(sb.files[i].name, name) == 0) {
            return false;
        }
    }
    
    scooterfs_dirent* new_file = &sb.files[sb.file_count];
    strcpy(new_file->name, name);
    new_file->inode_number = sb.file_count + 1;
    new_file->type = is_directory ? 2 : 1;
    new_file->size = 0;
    new_file->created_time = scooterfs_get_time();
    new_file->modified_time = new_file->created_time;
    new_file->is_deleted = false;
    
    sb.file_count++;
    return true;
}

bool scooterfs_delete_file(const char* name) {
    for (size_t i = 0; i < sb.file_count; i++) {
        if (!sb.files[i].is_deleted && strcmp(sb.files[i].name, name) == 0) {
            sb.files[i].is_deleted = true;
            return true;
        }
    }
    return false;
}

bool scooterfs_list_directory(scooterfs_dirent* entries, size_t* count) {
    *count = 0;
    for (size_t i = 0; i < sb.file_count; i++) {
        if (!sb.files[i].is_deleted) {
            entries[*count] = sb.files[i];
            (*count)++;
        }
    }
    return true;
}

bool scooterfs_format(void) {
    memset(sb.block_bitmap, 0, sb.total_blocks / 8);
    sb.block_bitmap[0] = 1; // Mark superblock as used
    
    // Create root directory
    scooterfs_dirent root_dir = {
        .name = "/",
        .inode_number = sb.root_inode,
        .type = 2, // Directory
        .size = 0,
        .created_time = 0,
        .modified_time = 0
    };
    
    memcpy(fs_buffer, &root_dir, sizeof(scooterfs_dirent));
    // TODO: Implement actual disk writing
    
    return true;
} 