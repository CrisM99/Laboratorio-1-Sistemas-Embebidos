
#include "filesystem.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Declaración de la función de carga
int load_files_into_system(BattleFS *fs, const char *dir_path);

void print_help() {
    printf("\n=== BattleFS - Sistema de archivos comprimidos ===\n");
    printf("Comandos disponibles:\n");
    printf("  init                     - Inicializa un sistema nuevo\n");
    printf("  load_dir <directorio>    - Carga todos los archivos de un directorio\n");
    printf("  create <archivo>         - Añade un archivo al sistema\n");
    printf("  read <archivo>           - Muestra contenido de un archivo\n");
    printf("  delete <archivo>         - Elimina un archivo\n");
    printf("  list                     - Lista todos los archivos\n");
    printf("  save <nombre>            - Guarda el sistema\n");
    printf("  load <nombre>            - Carga un sistema\n");
    printf("  exit                     - Salir\n");
    printf("  help                     - Muestra esta ayuda\n");
}

int main() {
    BattleFS *fs = NULL;
    char command[256];
    char arg1[256];
    char arg2[256];
    
    printf("=== BattleFS - Sistema de archivos comprimidos ===\n");
    printf("Escribe 'help' para ver los comandos disponibles\n");
    
    while (1) {
        printf("\nBattleFS> ");
        if (!fgets(command, sizeof(command), stdin)) break;
        
        int args = sscanf(command, "%s %s %s", command, arg1, arg2);

        
        if (strcmp(command, "init") == 0) {
            if (fs) battlefs_free(fs);
            fs = battlefs_init("default");
            printf("Sistema inicializado.\n");
        }
        else if (strcmp(command, "load_dir") == 0 && args >= 2) {
            if (!fs) {
                printf("Error: Primero inicializa el sistema con 'init'\n");
            } else {
                int loaded = load_files_into_system(fs, arg1);
                if (loaded >= 0) {
                    printf("Se cargaron %d archivos desde '%s'\n", loaded, arg1);
                    printf("El valor de arg1 es:%c\n",arg1);
                    printf("El valor de arg2 es:%c\n",arg2);
                } else {
                    printf("Error al cargar archivos\n");
                    printf("El valor de arg1 es:%c\n",arg1);
                    printf("El valor de arg2 es:%c\n",arg2);
                }
            }
        }
        else if (strcmp(command, "create") == 0 && args >= 2) {
            if (!fs) {
                printf("Error: Sistema no inicializado. Use 'init' primero.\n");
            } else if (battlefs_create(fs, arg1) == 0) {
                printf("Archivo '%s' creado y comprimido.\n", arg1);
            } else {
                printf("Error al crear el archivo '%s'.\n", arg1);
            }
        }
        else if (strcmp(command, "read") == 0 && args >= 2) {
            if (!fs) {
                printf("Error: Sistema no inicializado. Use 'init' primero.\n");
            } else if (battlefs_read(fs, arg1) != 0) {
                printf("Error al leer el archivo '%s'.\n", arg1);
            }
        }
        else if (strcmp(command, "delete") == 0 && args >= 2) {
            if (!fs) {
                printf("Error: Sistema no inicializado. Use 'init' primero.\n");
            } else if (battlefs_delete(fs, arg1) == 0) {
                printf("Archivo '%s' eliminado.\n", arg1);
            } else {
                printf("Error al eliminar el archivo '%s'.\n", arg1);
            }
        }
        else if (strcmp(command, "list") == 0) {
            if (!fs) {
                printf("Error: Sistema no inicializado. Use 'init' primero.\n");
            } else {
                battlefs_list(fs);
            }
        }
        else if (strcmp(command, "save") == 0 && args >= 2) {
            if (!fs) {
                printf("Error: Sistema no inicializado. Use 'init' primero.\n");
            } else if (battlefs_save(fs, arg1) == 0) {
                printf("Sistema guardado como '%s'.\n", arg1);
            } else {
                printf("Error al guardar el sistema.\n");
            }
        }
        else if (strcmp(command, "load") == 0 && args >= 2) {
            if (fs) battlefs_free(fs);
            fs = battlefs_load(arg1);
            if (fs) {
                printf("Sistema '%s' cargado correctamente.\n", arg1);
            } else {
                printf("Error al cargar el sistema '%s'.\n", arg1);
            }
        }
        else if (strcmp(command, "exit") == 0) {
            if (fs) battlefs_free(fs);
            break;
        }
        else if (strcmp(command, "help") == 0) {
            print_help();
        }
        else {
            printf("Comando desconocido. Escribe 'help' para ayuda.\n");
        }
    }
    
    return 0;
}