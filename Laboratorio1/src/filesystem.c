
#define _POSIX_C_SOURCE 200809L
#include "filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static void print_entry(const char *filename, void *value) {
    FileEntry *entry = (FileEntry*)value;
    printf("- %s (%zu bytes -> %zu bytes)\n", 
           filename, entry->original_size, entry->compressed_size);
}

static void free_entry(const char *filename, void *value) {
    (void)filename;
    FileEntry *entry = (FileEntry*)value;
    free(entry->compressed_data);
    free(entry);
}

BattleFS* battlefs_init(const char *name) {
    if (!name) return NULL;
    
    BattleFS *fs = calloc(1, sizeof(BattleFS));
    if (!fs) return NULL;
    
    fs->index = bplus_tree_init();
    if (!fs->index) {
        free(fs);
        return NULL;
    }
    
    fs->name = strdup(name);
    if (!fs->name) {
        bplus_tree_free(fs->index);
        free(fs);
        return NULL;
    }
    
    fs->total_files = 0;
    fs->total_compressed_size = 0;
    fs->total_original_size = 0;
    
    return fs;
}

int battlefs_create(BattleFS *fs, const char *filename) {
    if (!fs || !filename) return -1;

    if (bplus_tree_search(fs->index, filename)) {
        fprintf(stderr, "Error: Archivo ya existe\n");
        return -1;
    }

    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error al abrir archivo");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size <= 0) {
        fclose(file);
        return -1;
    }

    uint8_t *file_data = malloc(file_size);
    if (!file_data) {
        fclose(file);
        return -1;
    }

    if (fread(file_data, 1, file_size, file) != (size_t)file_size) {
        free(file_data);
        fclose(file);
        return -1;
    }
    fclose(file);

    size_t compressed_size;
    uint8_t *compressed_data = lzw_compress(file_data, file_size, &compressed_size);
    free(file_data);
    if (!compressed_data) return -1;

    FileEntry *entry = malloc(sizeof(FileEntry));
    if (!entry) {
        free(compressed_data);
        return -1;
    }

    entry->compressed_data = compressed_data;
    entry->compressed_size = compressed_size;
    entry->original_size = file_size;

    bplus_tree_insert(fs->index, filename, entry);
    fs->total_files++;
    fs->total_compressed_size += compressed_size;
    fs->total_original_size += file_size;

    return 0;
}

int battlefs_read(BattleFS *fs, const char *filename) {
    if (!fs || !filename) return -1;

    FileEntry *entry = bplus_tree_search(fs->index, filename);
    if (!entry) {
        fprintf(stderr, "Error: Archivo no encontrado\n");
        return -1;
    }

    size_t decompressed_size;
    uint8_t *decompressed = lzw_decompress(entry->compressed_data, 
                                         entry->compressed_size, 
                                         &decompressed_size);
    if (!decompressed) return -1;

    fwrite(decompressed, 1, decompressed_size, stdout);
    free(decompressed);
    return 0;
}

int battlefs_delete(BattleFS *fs, const char *filename) {
    if (!fs || !filename) return -1;

    FileEntry *entry = bplus_tree_search(fs->index, filename);
    if (!entry) {
        fprintf(stderr, "Error: Archivo no encontrado\n");
        return -1;
    }

    fs->total_files--;
    fs->total_compressed_size -= entry->compressed_size;
    fs->total_original_size -= entry->original_size;

    free(entry->compressed_data);
    free(entry);
    return bplus_tree_delete(fs->index, filename);
}

void battlefs_list(BattleFS *fs) {
    if (!fs) return;

    printf("\n=== Sistema: %s ===\n", fs->name);
    printf("Archivos totales: %zu\n", fs->total_files);
    printf("Tamaño original: %zu bytes\n", fs->total_original_size);
    printf("Tamaño comprimido: %zu bytes\n", fs->total_compressed_size);
    printf("Tasa de compresión: %.2f%%\n", 
           (100.0 - (100.0 * fs->total_compressed_size / fs->total_original_size)));
    printf("\nContenido:\n");
    bplus_tree_list(fs->index, print_entry);
}

int battlefs_save(BattleFS *fs, const char *system_name) {
    (void)fs;
    (void)system_name;
    printf("Funcionalidad de guardado no implementada aún\n");
    return 0;
}

BattleFS* battlefs_load(const char *system_name) {
    (void)system_name;
    printf("Funcionalidad de carga no implementada aún\n");
    return NULL;
}

void battlefs_free(BattleFS *fs) {
    if (!fs) return;
    
    if (fs->index) {
        bplus_tree_list(fs->index, free_entry);
        bplus_tree_free(fs->index);
    }
    
    free(fs->name);
    free(fs);
}