cmake_minimum_required(VERSION 3.10)
project(battlefs C)

# Configuración básica
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Flags del compilador
add_compile_options(-Wall -Wextra)
add_definitions(-D_POSIX_C_SOURCE=200809L)

# Directorios de inclusión
include_directories(src)

# Fuentes del proyecto
set(SRC
    src/main.c
    src/filesystem.c
    src/compression.c
    src/tree.c
    src/file_loader.c
)

# Ejecutable principal
add_executable(battlefs ${SRC})

# Opcional: Instalación (descomenta si lo necesitas)
# install(TARGETS battlefs DESTINATION bin)
