#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "util/file_helpers.h"
#include "util/string_helpers.h"

typedef struct FileTree FileTree;
typedef struct FileNode FileNode;

struct FileNode {
    // Data
    char name[256];
    ino_t inode;
    size_t file_size; // Size of this individual file
    size_t complete_size; // Includes every child size
    size_t depth; // Depth of node from root
    time_t last_access_time; // Last access time
    // Tree information
    FileNode* parent;
    // Children are stored as a linked list
    FileNode* first_child; // Pointer to the first child
    FileNode* last_child; // Pointer to the last child
    FileNode* next_sibling; // Pointer to the next sibling
    FileNode* previous_sibling; // Pointer to the previous sibling
    size_t _linear_index;
};

// Create a new file tree, representing a filesystem with sizes
FileNode* file_node_new();

// Free the entire tree below, starting with this root node
void file_node_free_all(FileNode* node);

// Set the name of the file node
void file_node_set_name(FileNode* node, char* name);

// Add a child to a file node
FileNode* file_tree_add_child(FileNode* parent);

// Save the file tree to file (used as cache)
FileNode* file_tree_load(char* filename);

// Load the file tree from file (used as cache)
void file_tree_save(FileNode* root, char* filename);

// Count the amount of nodes in the tree, ie all decendents and neighbours of node
size_t file_tree_count_nodes(FileNode* node);

// Find a node in the tree with a specific filepath and inode
FileNode* file_tree_find(FileNode* node, ino_t inode);