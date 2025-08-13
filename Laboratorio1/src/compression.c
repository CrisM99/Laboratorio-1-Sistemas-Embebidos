
#include "compression.h"
#include <stdio.h>
#include <string.h>

uint8_t* lzw_compress(const uint8_t *input, size_t input_size, size_t *output_size) {
    LZWDictionary dict;
    // Inicializar diccionario
    for (int i = 0; i < 256; i++) {
        dict.entries[i].value = i;
        for (int j = 0; j < 256; j++) {
            dict.entries[i].next[j] = LZW_DICT_SIZE;
        }
    }
    dict.size = 256;

    uint16_t *output = malloc(input_size * sizeof(uint16_t));
    if (!output) return NULL;

    size_t output_pos = 0;
    uint16_t current_code = input[0];

    for (size_t i = 1; i < input_size; i++) {
        uint8_t next_char = input[i];
        uint16_t next_code = dict.entries[current_code].next[next_char];

        if (next_code < LZW_DICT_SIZE) {
            current_code = next_code;
        } else {
            if (output_pos >= input_size) {
                uint16_t *new_output = realloc(output, (output_pos + 1) * sizeof(uint16_t));
                if (!new_output) {
                    free(output);
                    return NULL;
                }
                output = new_output;
            }
            output[output_pos++] = current_code;
            
            if (dict.size < LZW_DICT_SIZE) {
                dict.entries[current_code].next[next_char] = dict.size;
                dict.entries[dict.size].value = next_char;
                dict.size++;
            }
            
            current_code = next_char;
        }
    }

    output[output_pos++] = current_code;
    *output_size = output_pos * sizeof(uint16_t);
    return (uint8_t*)output;
}

uint8_t* lzw_decompress(const uint8_t *input, size_t input_size, size_t *output_size) {
    if (!input || input_size == 0 || !output_size) return NULL;

    LZWDictionary dict;
    uint8_t *output = NULL;
    *output_size = 0;
    
    // Inicializar diccionario
    for (int i = 0; i < 256; i++) {
        dict.entries[i].value = i;
        memset(dict.entries[i].next, 0xFF, sizeof(dict.entries[i].next));
    }
    dict.size = 256;

    const uint16_t *codes = (const uint16_t *)input;
    size_t num_codes = input_size / sizeof(uint16_t);
    size_t output_alloc = num_codes * 2;
    
    output = malloc(output_alloc);
    if (!output) return NULL;

    for (size_t i = 0; i < num_codes; i++) {
        if (*output_size + 1 > output_alloc) {
            output_alloc *= 2;
            uint8_t *new_output = realloc(output, output_alloc);
            if (!new_output) {
                free(output);
                return NULL;
            }
            output = new_output;
        }
        
        uint16_t code = codes[i];
        if (code >= dict.size) {
            free(output);
            return NULL; // Código inválido
        }
        
        output[(*output_size)++] = dict.entries[code].value;
    }

    return output;
}

int compress_file(const char *filename, uint8_t **compressed_data, size_t *compressed_size) {
    FILE *file = fopen(filename, "rb");
    if (!file) return -1;

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

    size_t read = fread(file_data, 1, file_size, file);
    fclose(file);

    if (read != (size_t)file_size) {
        free(file_data);
        return -1;
    }

    *compressed_data = lzw_compress(file_data, file_size, compressed_size);
    free(file_data);

    return (*compressed_data) ? 0 : -1;
}

int decompress_to_file(const char *filename, const uint8_t *compressed_data, size_t compressed_size) {
    if (!filename || !compressed_data) return -1;

    size_t output_size;
    uint8_t *decompressed = lzw_decompress(compressed_data, compressed_size, &output_size);
    if (!decompressed) return -1;

    FILE *file = fopen(filename, "wb");
    if (!file) {
        free(decompressed);
        return -1;
    }

    size_t written = fwrite(decompressed, 1, output_size, file);
    fclose(file);
    free(decompressed);

    return (written == output_size) ? 0 : -1;
}