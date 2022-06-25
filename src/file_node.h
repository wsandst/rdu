#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/file_helpers.h"

typedef struct FileTree FileTree;
typedef struct FileNode FileNode;

struct FileNode {
    char name[256];
    size_t file_size; // Size of this individual file
    size_t complete_size; // Includes every child size
    // Children are a linked list
    FileNode* parent;
    FileNode* first_child; // Pointer to the first child
    FileNode* last_child; // Pointer to the last child
    FileNode* next_sibling; // Pointer to the
    FileNode* previous_sibling;
    size_t _linear_index;
};

// Create a new file tree, representing a filesystem with sizes
FileNode* file_node_new();

// Free the entire tree below, starting with this root node
void file_node_free_all(FileNode* node);

// Add a child to a file node
FileNode* file_tree_add_child(FileNode* parent);

// Save the file tree to file (used as cache)
FileNode* file_tree_load(char* filename);

// Load the file tree from file (used as cache)
void file_tree_save(FileNode* root, char* filename);

// Count the amount of nodes in the tree, ie all decendents and neighbours of node
size_t file_tree_count_nodes(FileNode* node);
