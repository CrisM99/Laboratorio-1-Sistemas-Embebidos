
#define _POSIX_C_SOURCE 200809L
#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void free_node_recursive(BPlusNode *node) {
    if (!node) return;
    
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            free_node_recursive(node->pointers[i]);
        }
    }
    
    for (int i = 0; i < node->num_keys; i++) {
        free(node->keys[i]);
    }
    
    free(node);
}

BPlusTree* bplus_tree_init() {
    BPlusTree *tree = calloc(1, sizeof(BPlusTree));
    if (!tree) return NULL;
    return tree;
}

static BPlusNode* create_node(int is_leaf) {
    BPlusNode *node = calloc(1, sizeof(BPlusNode));
    if (!node) return NULL;
    
    node->is_leaf = is_leaf;
    return node;
}

static int find_key_index(BPlusNode *node, const char *key) {
    int i = 0;
    while (i < node->num_keys && strcmp(key, node->keys[i]) > 0) {
        i++;
    }
    return i;
}

static void insert_into_leaf(BPlusNode *leaf, const char *key, void *value) {
    int pos = find_key_index(leaf, key);
    
    for (int i = leaf->num_keys; i > pos; i--) {
        leaf->keys[i] = leaf->keys[i-1];
        leaf->pointers[i] = leaf->pointers[i-1];
    }
    
    leaf->keys[pos] = strdup(key);
    leaf->pointers[pos] = value;
    leaf->num_keys++;
}

static BPlusNode* split_leaf(BPlusNode *leaf) {
    BPlusNode *new_leaf = create_node(1);
    if (!new_leaf) return NULL;
    
    int split_pos = leaf->num_keys / 2;
    
    for (int i = split_pos; i < leaf->num_keys; i++) {
        new_leaf->keys[i - split_pos] = leaf->keys[i];
        new_leaf->pointers[i - split_pos] = leaf->pointers[i];
        leaf->keys[i] = NULL;
        leaf->pointers[i] = NULL;
    }
    
    leaf->num_keys = split_pos;
    new_leaf->num_keys = leaf->num_keys - split_pos;
    new_leaf->next = leaf->next;
    leaf->next = new_leaf;
    new_leaf->parent = leaf->parent;
    
    return new_leaf;
}

static void insert_into_parent(BPlusTree *tree, BPlusNode *left, BPlusNode *right, const char *key);

static BPlusNode* insert_into_node(BPlusTree *tree, BPlusNode *node, int index, 
                                 const char *key, BPlusNode *right) {
    for (int i = node->num_keys; i > index; i--) {
        node->keys[i] = node->keys[i-1];
        node->pointers[i+1] = node->pointers[i];
    }
    
    node->keys[index] = strdup(key);
    node->pointers[index+1] = right;
    node->num_keys++;
    
    if (node->num_keys >= ORDER) {
        BPlusNode *new_node = create_node(0);
        if (!new_node) return NULL;
        
        int split_pos = node->num_keys / 2;
        char *split_key = node->keys[split_pos];
        
        for (int i = split_pos + 1; i < node->num_keys; i++) {
            new_node->keys[i - (split_pos + 1)] = node->keys[i];
            new_node->pointers[i - (split_pos + 1)] = node->pointers[i];
            ((BPlusNode*)node->pointers[i])->parent = new_node;
            node->keys[i] = NULL;
            node->pointers[i] = NULL;
        }
        
        new_node->pointers[node->num_keys - (split_pos + 1)] = node->pointers[node->num_keys];
        ((BPlusNode*)node->pointers[node->num_keys])->parent = new_node;
        node->pointers[node->num_keys] = NULL;
        
        new_node->num_keys = node->num_keys - (split_pos + 1);
        node->num_keys = split_pos;
        new_node->parent = node->parent;
        
        if (node == tree->root) {
            BPlusNode *new_root = create_node(0);
            if (!new_root) return NULL;
            
            new_root->keys[0] = split_key;
            new_root->pointers[0] = node;
            new_root->pointers[1] = new_node;
            new_root->num_keys = 1;
            node->parent = new_root;
            new_node->parent = new_root;
            tree->root = new_root;
        } else {
            insert_into_parent(tree, node, new_node, split_key);
        }
    }
    
    return node;
}

static void insert_into_parent(BPlusTree *tree, BPlusNode *left, BPlusNode *right, const char *key) {
    BPlusNode *parent = left->parent;
    
    if (!parent) {
        BPlusNode *new_root = create_node(0);
        if (!new_root) return;
        
        new_root->keys[0] = strdup(key);
        new_root->pointers[0] = left;
        new_root->pointers[1] = right;
        new_root->num_keys = 1;
        left->parent = new_root;
        right->parent = new_root;
        tree->root = new_root;
        return;
    }
    
    int index = find_key_index(parent, key);
    insert_into_node(tree, parent, index, key, right);
}

void bplus_tree_insert(BPlusTree *tree, const char *key, void *value) {
    if (!tree || !key) return;
    
    if (!tree->root) {
        tree->root = create_node(1);
        if (!tree->root) return;
        
        tree->root->keys[0] = strdup(key);
        tree->root->pointers[0] = value;
        tree->root->num_keys = 1;
        return;
    }
    
    BPlusNode *node = tree->root;
    while (!node->is_leaf) {
        int i = find_key_index(node, key);
        node = node->pointers[i];
    }
    
    insert_into_leaf(node, key, value);
    
    if (node->num_keys >= ORDER) {
        BPlusNode *new_leaf = split_leaf(node);
        if (!new_leaf) return;
        
        char *new_key = new_leaf->keys[0];
        insert_into_parent(tree, node, new_leaf, new_key);
    }
}

void* bplus_tree_search(BPlusTree *tree, const char *key) {
    if (!tree || !tree->root || !key) return NULL;
    
    BPlusNode *node = tree->root;
    while (!node->is_leaf) {
        int i = find_key_index(node, key);
        node = node->pointers[i];
    }
    
    for (int i = 0; i < node->num_keys; i++) {
        if (strcmp(key, node->keys[i]) == 0) {
            return node->pointers[i];
        }
    }
    
    return NULL;
}

static void remove_entry(BPlusNode *node, int index) {
    free(node->keys[index]);
    
    if (node->is_leaf) {
        for (int i = index; i < node->num_keys - 1; i++) {
            node->keys[i] = node->keys[i+1];
            node->pointers[i] = node->pointers[i+1];
        }
    } else {
        for (int i = index; i < node->num_keys - 1; i++) {
            node->keys[i] = node->keys[i+1];
            node->pointers[i+1] = node->pointers[i+2];
        }
    }
    
    node->num_keys--;
}

int bplus_tree_delete(BPlusTree *tree, const char *key) {
    if (!tree || !tree->root || !key) return -1;
    
    BPlusNode *node = tree->root;
    while (!node->is_leaf) {
        int i = find_key_index(node, key);
        node = node->pointers[i];
    }
    
    int index = -1;
    for (int i = 0; i < node->num_keys; i++) {
        if (strcmp(key, node->keys[i]) == 0) {
            index = i;
            break;
        }
    }
    
    if (index == -1) return -1;
    
    remove_entry(node, index);
    return 0;
}

void bplus_tree_free(BPlusTree *tree) {
    if (!tree) return;
    free_node_recursive(tree->root);
    free(tree);
}

void bplus_tree_list(BPlusTree *tree, void (*callback)(const char *key, void *value)) {
    if (!tree || !tree->root || !callback) return;
    
    BPlusNode *node = tree->root;
    while (!node->is_leaf) {
        node = node->pointers[0];
    }
    
    while (node) {
        for (int i = 0; i < node->num_keys; i++) {
            callback(node->keys[i], node->pointers[i]);
        }
        node = node->next;
    }
}