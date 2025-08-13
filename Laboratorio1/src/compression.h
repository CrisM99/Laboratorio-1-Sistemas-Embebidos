
#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#define LZW_DICT_SIZE 4096
#define LZW_MAX_CODE (LZW_DICT_SIZE - 1)

typedef struct {
    uint16_t next[256];
    uint8_t value;
} LZWDictEntry;

typedef struct {
    LZWDictEntry entries[LZW_DICT_SIZE];
    uint16_t size;
} LZWDictionary;

uint8_t* lzw_compress(const uint8_t *input, size_t input_size, size_t *output_size);
uint8_t* lzw_decompress(const uint8_t *input, size_t input_size, size_t *output_size);
int compress_file(const char *filename, uint8_t **compressed_data, size_t *compressed_size);
int decompress_to_file(const char *filename, const uint8_t *compressed_data, size_t compressed_size);

#endif