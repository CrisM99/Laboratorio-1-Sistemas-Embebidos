
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "tree.h"
#include "compression.h"
#include <stdint.h>
#include <stddef.h>
#include <sys/stat.h>

typedef struct {
    uint8_t *compressed_data;
    size_t compressed_size;
    size_t original_size;
} FileEntry;

typedef struct {
    BPlusTree *index;
    char *name;
    size_t total_files;
    size_t total_compressed_size;
    size_t total_original_size;
} BattleFS;

BattleFS* battlefs_init(const char *name);
int battlefs_create(BattleFS *fs, const char *filename);
int battlefs_read(BattleFS *fs, const char *filename);
int battlefs_delete(BattleFS *fs, const char *filename);
void battlefs_list(BattleFS *fs);
int battlefs_save(BattleFS *fs, const char *system_name);
BattleFS* battlefs_load(const char *system_name);
void battlefs_free(BattleFS *fs);

#endif