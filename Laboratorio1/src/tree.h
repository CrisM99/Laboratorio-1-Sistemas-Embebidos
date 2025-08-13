
#ifndef TREE_H
#define TREE_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define ORDER 4
#define MIN_KEYS (ORDER / 2)

typedef struct BPlusNode {
    int is_leaf;
    int num_keys;
    char *keys[ORDER];
    void *pointers[ORDER + 1];
    struct BPlusNode *parent;
    struct BPlusNode *next;
} BPlusNode;

typedef struct {
    BPlusNode *root;
} BPlusTree;

BPlusTree* bplus_tree_init();
void bplus_tree_free(BPlusTree *tree);
void bplus_tree_insert(BPlusTree *tree, const char *key, void *value);
void* bplus_tree_search(BPlusTree *tree, const char *key);
int bplus_tree_delete(BPlusTree *tree, const char *key);
void bplus_tree_list(BPlusTree *tree, void (*callback)(const char *key, void *value));

#endif