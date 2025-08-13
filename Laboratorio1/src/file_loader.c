
#define _POSIX_C_SOURCE 200809L
#include "filesystem.h"
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

int load_files_into_system(BattleFS *fs, const char *dir_path) {
    DIR *dir;
    struct dirent *ent;
    int loaded_files = 0;
    char full_path[PATH_MAX];
    
    // Construir ruta completa manualmente
    if (dir_path[0] == '/') {
        // Ruta absoluta
        strncpy(full_path, dir_path, sizeof(full_path));
    } else {
        // Ruta relativa - agregar al directorio actual
        char cwd[PATH_MAX];
        if (!getcwd(cwd, sizeof(cwd))) {
            perror("Error al obtener directorio actual");
            return -1;
        }
        snprintf(full_path, sizeof(full_path), "%s/%s", cwd, dir_path);
    }

    // Eliminar barras duplicadas (opcional pero recomendado)
    for (char *p = full_path; *p; p++) {
        if (*p == '/' && *(p+1) == '/') {
            memmove(p, p+1, strlen(p));
        }
    }

    printf("Cargando desde: %s\n", full_path);
    
    // Verificar que es un directorio
    struct stat st;
    if (stat(full_path, &st) == -1 || !S_ISDIR(st.st_mode)) {
        fprintf(stderr, "Error: '%s' no es un directorio válido\n", full_path);
        return -1;
    }

    if ((dir = opendir(full_path)) == NULL) {
        fprintf(stderr, "Error al abrir directorio: %s\n", strerror(errno));
        return -1;
    }

    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }

        char file_path[PATH_MAX * 2];
        snprintf(file_path, sizeof(file_path), "%s/%s", full_path, ent->d_name);
        
        if (stat(file_path, &st) == 0 && S_ISREG(st.st_mode)) {
            printf("Procesando: %s\n", ent->d_name);
            if (battlefs_create(fs, file_path) == 0) {
                loaded_files++;
            } else {
                fprintf(stderr, "Error al cargar: %s\n", ent->d_name);
            }
        }
    }
    
    closedir(dir);
    return loaded_files;
}