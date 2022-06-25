#include "file_node.h"

static FileNode* file_tree_linearize(FileNode* root);
static void file_tree_remap_recurse(FileNode* node, int* index, FileNode** node_pointers);

// Create a new file tree, representing a filesystem with sizes
FileNode* file_node_new() {
    return calloc(1, sizeof(FileNode));
}

// Free the entire tree below, starting with this root node
FileNode* file_node_free_all(FileNode* node) {
    if (node->next_sibling) {
        file_node_free_all(node->next_sibling);
    }
    if (node->first_child) {
        file_node_free_all(node->first_child);
    }
    free(node);
}

// Add a child to the parent node.
FileNode* file_tree_add_child(FileNode* parent) {
    FileNode* node = file_node_new();
    if (parent->first_child == NULL) {
        parent->first_child = node;
        parent->last_child = node;
    }
    else {
        parent->last_child->next_sibling = node;
        node->previous_sibling = parent->last_child;
        node->last_child = node;
    }
    return node;
}

// Load the file tree from file (used as cache)
FileNode* file_tree_load(char* filename) {
    //Vec*
}

// Save the file tree to file (used as cache)
void file_tree_save(FileNode* root, char* filename) {
    FileNode* linear_mem = file_tree_linearize(root);

    FILE* f = fopen(filename, "wb");
    fwrite(linear_mem, sizeof(FileNode), 100, f);
    fclose(f);
}

// Convert the dynamic tree into a linear array, remap the pointers to indices
static FileNode* file_tree_linearize(FileNode* root) {
    // We start out creating a linear array to pointers, then we can copy the data properly afterwards
    FileNode** node_pointers = calloc(100, sizeof(FileNode*));
    file_tree_remap_recurse(root, 1, node_pointers);

    // Convert the array of pointers into actual
    FileNode* linear_mem = calloc(100, sizeof(FileNode));
    for (int i = 0; i < 100; i++) {
        linear_mem[i] = *node_pointers[i];
    }
    free(node_pointers);

    return linear_mem;
}

// Remap the pointers in the tree to refer to an index in an array
// Warning: This is destructive to the original tree, use file_tree_delinearize() to go back
static void file_tree_remap_recurse(FileNode* node, int* index, FileNode** node_pointers) {
    // Every pointer needs to be mapped to an index, then copied to the array
    while (node) {
        if (node->first_child) {
            file_tree_remap_recurse(node, index, node_pointers);
        }
        // Write to array here and remap

        FileNode* next_sibling = node->next_sibling;

        // Change all pointer references to this node into an index
        if (node->previous_sibling) {
            node->previous_sibling->next_sibling = index;
        }
        if (node->parent->first_child == node) {
            node->parent->first_child = index;
        }
        if (node->parent->last_child == node) {
            node->parent->last_child = index;
        }

        // Copy a pointer to this node into the position of the new node index
        node_pointers[*index] = node;

        *index++;
    }
}
